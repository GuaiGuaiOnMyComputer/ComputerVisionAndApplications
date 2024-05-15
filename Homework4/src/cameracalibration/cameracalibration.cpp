#include <opencv2/opencv.hpp>
#include <execution>
#include <algorithm>
#include "cameracalibration.hpp"
#include "xyzio.hpp"

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

    cv::Mat CameraCalibration::find_panel_homography(const std::vector<XyzIo::Coor2D_f>& imagePoints, const std::vector<XyzIo::Coor2D_f>& panelPoints)
    {
        return cv::findHomography(panelPoints, imagePoints, 0);
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
        const cv::Mat_<double> lastColumnOfV(vt.t(), cv::Range::all(), cv::Range(5, 6));
        omegaMatrix(0, 0) = lastColumnOfV(0);
        omegaMatrix(0, 1) = lastColumnOfV(1);
        omegaMatrix(0, 2) = lastColumnOfV(2);
        omegaMatrix(1, 0) = omegaMatrix(0, 1);
        omegaMatrix(1, 1) = lastColumnOfV(3);
        omegaMatrix(1, 2) = lastColumnOfV(4);
        omegaMatrix(2, 0) = omegaMatrix(0, 2);
        omegaMatrix(2, 1) = omegaMatrix(1, 2);
        omegaMatrix(2, 2) = lastColumnOfV(5);
        w.release();
        u.release();
        vt.release();

        cv::Mat_<double> omegaMat_inv = omegaMatrix.inv();
        omegaMat_inv /= omegaMat_inv.at<double>(2, 2); // normalize the omega matrix by w33
        /*
        K = [
            a, b, c
            0, d, e
            0, 0, 1
        ]
        Assume no skew
        k = [
            a, 0, c
            0, d, e
            0, 0, 1
        ]
        */
        cv::Mat k(3, 3, CV_64FC1, cv::Scalar(0));
        cv::Mat_<double> &k_ = (cv::Mat_<double>&)k;
        k_(0, 2) = omegaMat_inv(0, 2); // c
        k_(1, 2) = omegaMat_inv(1, 2); // e
        k_(0, 0) = abs(omegaMat_inv(0, 0) - k_(0, 2) * k_(0, 2)); // a
        k_(1, 1) = abs(omegaMat_inv(1, 1) - k_(1, 2) * k_(1, 2)); // d
        k_(2, 2) = 1; // the bottom right element in k is 1
        return k;
    }

    cv::Mat CameraCalibration::_get_the_crazy_mat_that_mulitplies_with_omega_and_i_have_no_idea_how_it_works(const std::vector<cv::Mat> &panelHomographyMats)
    {
        cv::Mat theCrazyMatNotatedAsStacksOfVs(6, 6, CV_64FC1, cv::Scalar(1314520));
        cv::Mat_<double> &theCrazyMatNotatedAsStacksOfVs_ = (cv::Mat_<double> &)theCrazyMatNotatedAsStacksOfVs;

        // each iteration stacks a v into theCrazyMatNotatedAsStacksOfVs, which is 2 rows in theCrazyMatNotatedAsStacksOfVs
        // please refere to the "CameraCalibration" chapter see "Absolute Conics" for detains about this algorithm.
        for (size_t i = 0; i < Panel::PANEL_COUNT; i++)
        {
            const cv::Mat_<double> &h = panelHomographyMats[i];

            // odd rows in theCrazyMatNotatedAsStacksOfVs, which is the first row in a v
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 0) = h(0, 0) * h(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 1) = h(0, 0) * h(1, 1) + h(0, 1) * h(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 2) = h(0, 0) * h(1, 2) + h(0, 2) * h(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 3) = h(0, 1) * h(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 4) = h(0, 1) * h(1, 2) + h(0, 2) * h(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i, 5) = h(0, 2) * h(1, 2);

            // even rows in theCrazyMatNotatedAsStacksOfVs, which is the second row in a v
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 0) = h(0, 0) * h(0, 0) - h(1, 0) * h(1, 0);
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 1) = 2 * (h(0, 0) * h(0, 1) - h(1, 0) * h(1, 1));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 2) = 2 * (h(0, 0) * h(0, 2) - h(1, 0) * h(1, 2));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 3) = h(0, 1) * h(0, 1) - h(1, 1) * h(1, 1);
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 4) = 2 * (h(0, 1) * h(0, 2) - h(1, 1) * h(1, 2));
            theCrazyMatNotatedAsStacksOfVs_(2 * i + 1, 5) = h(0, 2) * h(0, 2) - h(1, 2) * h(1, 2);
        }
        return theCrazyMatNotatedAsStacksOfVs;
    }
}