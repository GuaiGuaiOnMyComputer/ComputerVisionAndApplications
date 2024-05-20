#include <opencv2/opencv.hpp>
#include <execution>
#include <algorithm>
#include "cameracalibration.hpp"
#include "xyzio.hpp"
#include "cvmatio.hpp"

#if true
template class std::vector<hw4::XyzIo::Coor2D_f>;
template class std::vector<hw4::XyzIo::Coor3D_f>;
template class std::vector<cv::Mat>;
template class std::array<cv::Mat, hw4::CameraCalibration::PANEL_COUNT>;
template class std::array < std::vector<hw4::XyzIo::Coor2D_f>, hw4::CameraCalibration::PANEL_COUNT>;
template class cv::Mat_<float>;
template class cv::Mat_<double>;
#endif

namespace hw4
{
    using Panel = CameraCalibration::Panel;

    /// @brief So apparently ```cv::findHomography``` is bugged at this commit of opencv, so I have to implement it on my own.
    /// @param imagePoints The 2D coordinates of the 4 points in image coordinate system.
    /// @param panelPoints The 2D coordinates of the 4 points in local panel coordinate system.
    /// @return Homography matrix H, such that imagePoint = H * panelPoint
    cv::Mat CameraCalibration::find_panel_homography(const std::vector<XyzIo::Coor2D_f>& imagePoints, const std::vector<XyzIo::Coor2D_f>& panelPoints)
    {
        assert(imagePoints.size() >= 4 && panelPoints.size() >= 4);
        cv::Mat A(8, 8, CV_64FC1);
        cv::Vec<double, 2 * 4> x_panel, x_image;

        for (size_t i = 0; i < imagePoints.size(); i++)
        {
            cv::Mat_<double> &A_ = (cv::Mat_<double> &)A;
            A_(2 * i, 0) = panelPoints[i].x;
            A_(2 * i, 1) = panelPoints[i].y;
            A_(2 * i, 2) = 1;
            memset(&A_(2 * i, 3), 0, 3 * sizeof(double));
            A_(2 * i, 6) = -imagePoints[i].x * panelPoints[i].x;
            A_(2 * i, 7) = -imagePoints[i].x * panelPoints[i].y;

            memset(&A_(2 * i + 1, 0), 0, 3 * sizeof(double));
            A_(2 * i + 1, 3) = panelPoints[i].x;
            A_(2 * i + 1, 4) = panelPoints[i].y;
            A_(2 * i + 1, 5) = 1;
            A_(2 * i + 1, 6) = -imagePoints[i].y * panelPoints[i].x;
            A_(2 * i + 1, 7) = -panelPoints[i].y * imagePoints[i].y;

            x_image[2 * i] = imagePoints[i].x;
            x_panel[2 * i] = panelPoints[i].x;
            x_image[2 * i + 1] = imagePoints[i].y;
            x_panel[2 * i + 1] = panelPoints[i].y;
        }
        cv::Mat_<double> h = A.inv() * x_image;
        cv::Mat H(3, 3, CV_64FC1, cv::Scalar(1));
        memcpy(H.data, h.data, sizeof(double) * 8);

        H = H.reshape(0, 2, std::array<int32_t, 2>{3, 3}.data());
        return H;
    }

    std::vector<cv::Mat> CameraCalibration::find_panel_homography(const std::array<std::vector<XyzIo::Coor2D_f>, Panel::PANEL_COUNT> &imagePoints, const std::array<std::vector<XyzIo::Coor2D_f>, Panel::PANEL_COUNT> &panelPoints)
    {
        std::vector<cv::Mat> panelHomographyMats(Panel::PANEL_COUNT);
        const auto findPanelHomographyWrapper = [](const std::vector<XyzIo::Coor2D_f> &_imagePoints, const std::vector<XyzIo::Coor2D_f> &_panelPoints) -> cv::Mat {
            return CameraCalibration::find_panel_homography(_imagePoints, _panelPoints); 
        };
        std::transform(std::execution::seq, imagePoints.cbegin(), imagePoints.cend(), panelPoints.cbegin(), panelHomographyMats.begin(), findPanelHomographyWrapper);
        return panelHomographyMats;
    }

    cv::Mat CameraCalibration::get_k_matrix(const std::vector<cv::Mat>& panelHomographyMats)
    {
        const cv::Mat stacksOfVs = _get_the_crazy_mat_that_mulitplies_with_omega_and_i_have_no_idea_how_it_works(panelHomographyMats);
        cv::Mat_<double> omegaMatrix(3, 3, CV_64FC1);

        cv::Mat u, vt, w;
        cv::SVD::compute(stacksOfVs, w, u, vt);
        const cv::Mat v = vt.t();
        cv::Mat_<double> lastColumnOfV(v, cv::Range::all(), cv::Range(5, 6));
        omegaMatrix(0, 0) = lastColumnOfV(0);
        omegaMatrix(0, 1) = lastColumnOfV(1);
        omegaMatrix(0, 2) = lastColumnOfV(2);
        omegaMatrix(1, 0) = lastColumnOfV(1);
        omegaMatrix(1, 1) = lastColumnOfV(3);
        omegaMatrix(1, 2) = lastColumnOfV(4);
        omegaMatrix(2, 0) = lastColumnOfV(2);
        omegaMatrix(2, 1) = lastColumnOfV(4);
        omegaMatrix(2, 2) = lastColumnOfV(5);
        w.release();
        u.release();
        vt.release();

        cv::Mat_<double> omegaMat_inv = omegaMatrix.inv();
        /*
        K = [
            a, b, c
            0, d, e
            0, 0, 1
        ]
        */
        cv::Mat k(3, 3, CV_64FC1, cv::Scalar(0));
        cv::Mat_<double> &k_ = (cv::Mat_<double>&)k;
        omegaMat_inv /= omegaMat_inv(2, 2);

        const double c = omegaMat_inv(0, 2);
        const double e = omegaMat_inv(1, 2);
        const double d = sqrt(omegaMat_inv(1, 1) - e * e);
        const double b = (omegaMat_inv(0, 1) - c * e) / d;
        const double a = sqrt(omegaMat_inv(0, 0) - b * b - c * c);

        k_(0, 0) = a;
        k_(0, 1) = b;
        k_(0, 2) = c;
        k_(1, 1) = d;
        k_(1, 2) = e;
        k_(2, 2) = 1;

        return k;
    }

    cv::Mat CameraCalibration::get_rt_matrix(const cv::Mat &k_inverse, const cv::Mat &panelHomographyMat)
    {
        cv::Mat rT(3, 4, CV_64FC1, cv::Scalar(0));
        rT.colRange(0, 3) = k_inverse * panelHomographyMat;

        // normalize the first column in rT
        const double r1Length = cv::norm(rT.col(0));
        cv::normalize(rT.col(0), rT.col(0));

        // normalize the second column in rT as temporary r2
        cv::normalize(rT.col(1), rT.col(1));

        // the third column in rT is the cross product of the first two columns in rT
        rT.col(2) = rT.col(0).cross(rT.col(1));
        cv::normalize(rT.col(2), rT.col(2));

        // the second column of rT is the cross product of the first and third column in rT
        rT.col(1) = rT.col(0).cross(rT.col(2));

        // the forth column of rT is the t vector
        rT.col(3) = k_inverse * panelHomographyMat.col(2);
        rT.col(3) /= r1Length;

        hw4::CvMatIo::print_cv_mat<double, 6, 8>(rT, "rT");

        return rT;
    }

    std::vector<cv::Mat> CameraCalibration::get_rt_matrix(const cv::Mat &k, const std::vector<cv::Mat> &panelHomograhpyMat)
    {
        std::vector<cv::Mat> panelRtMats(Panel::PANEL_COUNT);
        for (size_t i = 0; i < Panel::PANEL_COUNT; i++)
        {
            panelRtMats[i] = get_rt_matrix(k.inv(), panelHomograhpyMat[i]);
        }
        return panelRtMats;
    }

    cv::Mat CameraCalibration::_get_the_crazy_mat_that_mulitplies_with_omega_and_i_have_no_idea_how_it_works(const std::vector<cv::Mat> &panelHomographyMats)
    {
        cv::Mat theCrazyMatNotatedAsStacksOfVs(6, 6, CV_64FC1, cv::Scalar(1314520));
        cv::Mat_<double> &theCrazyMatNotatedAsStacksOfVs_ = (cv::Mat_<double> &)theCrazyMatNotatedAsStacksOfVs;

        // each iteration stacks a v into theCrazyMatNotatedAsStacksOfVs, which is 2 rows in theCrazyMatNotatedAsStacksOfVs
        // please refere to the "CameraCalibration" chapter see "Absolute Conics" for detains about this algorithm.
        for (size_t i = 0; i < Panel::PANEL_COUNT; i++)
        {
            const cv::Mat hT = panelHomographyMats[i].t();
            const cv::Mat_<double> &hT_ = (cv::Mat_<double> &)hT;

            // odd rows in theCrazyMatNotatedAsStacksOfVs, which is the first row in a v
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 0) = hT_(0, 0) * hT_(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 1) = hT_(0, 0) * hT_(1, 1) + hT_(0, 1) * hT_(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 2) = hT_(0, 0) * hT_(1, 2) + hT_(0, 2) * hT_(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 3) = hT_(0, 1) * hT_(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 4) = hT_(0, 1) * hT_(1, 2) + hT_(0, 2) * hT_(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 5) = hT_(0, 2) * hT_(1, 2);

            // even rows in theCrazyMatNotatedAsStacksOfVs, which is the second row in a v
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 0) = hT_(0, 0) * hT_(0, 0) - hT_(1, 0) * hT_(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 1) = 2 * (hT_(0, 0) * hT_(0, 1) - hT_(1, 0) * hT_(1, 1));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 2) = 2 * (hT_(0, 0) * hT_(0, 2) - hT_(1, 0) * hT_(1, 2));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 3) = hT_(0, 1) * hT_(0, 1) - hT_(1, 1) * hT_(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 4) = 2 * (hT_(0, 1) * hT_(0, 2) - hT_(1, 1) * hT_(1, 2));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 5) = hT_(0, 2) * hT_(0, 2) - hT_(1, 2) * hT_(1, 2);
        }
        return theCrazyMatNotatedAsStacksOfVs;
    }
}