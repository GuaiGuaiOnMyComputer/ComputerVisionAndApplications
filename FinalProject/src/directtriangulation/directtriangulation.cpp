#include <array>
#include <opencv2/opencv.hpp>
#include "directtriangulation.hpp"
#include "cvmatdebug.hpp"

namespace finprj
{
    DirectTriangulation::DirectTriangulation(
        const std::array<double, 3 * 4> &leftRt, const std::array<double, 3 * 4> &rightRt, 
        const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK, 
        const std::array<double, 3 * 3>& fundementalMatrix) noexcept : 
            _leftRt{cv::Mat(3, 4, CV_64FC1, (void*)leftRt.data())}, _rightRt{cv::Mat(3, 4, CV_64FC1, (void*)rightRt.data())},
            _leftK{cv::Mat(3, 3, CV_64FC1, (void*)leftK.data())}, _rightK{cv::Mat(3, 3, CV_64FC1, (void*)rightK.data())},
            _leftP{_leftK * _leftRt}, _rightP{_rightK * _rightRt},
            _fundementalMatrix{cv::Mat(3, 3, CV_64FC1, (void*)fundementalMatrix.data())}
    {
        finprj::CvMatDebug::print_cv_mat<double, 5, 10>(_leftP, "_leftP");
    }

    DirectTriangulation::DirectTriangulation(const std::array<double, 4 * 3> &leftP, const std::array<double, 4 * 3> &rightP) noexcept :
        _leftRt{cv::Mat()}, _rightRt{cv::Mat()},
        _leftK{cv::Mat()}, _rightK{cv::Mat()},
        _leftP{cv::Mat(3, 4, CV_64FC1, (void*)leftP.data())}, _rightP{cv::Mat(3, 4, CV_64FC1, (void*)rightP.data())},
        _fundementalMatrix{cv::Mat()}
    {
    }


    cv::Point3d DirectTriangulation::LocalToWorld(const cv::Point2d& pointLeft, const cv::Point2d& pointRight)
    {
        std::array<double, 4 * 4> uvpMatBuffer;
        cv::Mat_<double> uvpMat(4, 4, (double *)uvpMatBuffer.data());
        DirectTriangulation::_get_uvp_mat(pointLeft, pointRight, this->_leftP, this->_rightP, uvpMat);
        finprj::CvMatDebug::print_cv_mat<double, 5, 10>(uvpMat, "uvp mat");

        std::array<double, 4 * 4> leftEigenVectorMatrixBuffer_;
        cv::Mat_<double> leftEigenVectorMatrix(4, 4, (double *)leftEigenVectorMatrixBuffer_.data());

        std::array<double, 4 * 4> rightEigenVectorMatrixBufferT_;
        cv::Mat_<double> rightEigenVectorMatrixT(4, 4, (double *)rightEigenVectorMatrixBufferT_.data());

        std::array<double, 4> eigenValuesBuffer_;
        cv::Mat_<double> eigenValues(4, 1, (double *)eigenValuesBuffer_.data());

        cv::SVD::compute(uvpMat, eigenValues, leftEigenVectorMatrix, rightEigenVectorMatrixT);
        finprj::CvMatDebug::print_cv_mat<double, 5, 10>(leftEigenVectorMatrix, "left eigenmat");
        finprj::CvMatDebug::print_cv_mat<double, 5, 10>(rightEigenVectorMatrixT, "right eigenmat");
        finprj::CvMatDebug::print_cv_mat<double, 5, 10>(eigenValues, "eigenvalues");
        const cv::Mat_<double> lastColumnOfRightEigenVectorMatrix = cv::Mat(rightEigenVectorMatrixT.t(), cv::Range::all(), cv::Range(3, 4));
        return cv::Point3d(
            lastColumnOfRightEigenVectorMatrix(0) / lastColumnOfRightEigenVectorMatrix(3), // world x coordinate
            lastColumnOfRightEigenVectorMatrix(1) / lastColumnOfRightEigenVectorMatrix(3), // world y coordiante
            lastColumnOfRightEigenVectorMatrix(2) / lastColumnOfRightEigenVectorMatrix(3)  // world z coordinate
        );
    }

    inline void DirectTriangulation::_get_uvp_mat(const cv::Point2d &pointLeft, const cv::Point2d &pointRight, const cv::Mat &pLeft, const cv::Mat &pRight, cv::Mat& out_uvpMat)
    {
        // check page 32 of Lecture08 handout
        constexpr double w = 1;
        const double uLeft = pointLeft.x / w;
        const double vLeft = pointLeft.y / w;
        const double uRight = pointRight.x / w;
        const double vRight = pointRight.y / w;

        // the first row
        cv::Mat firstRow = out_uvpMat.row(0);
        cv::multiply(pLeft.row(2), cv::Scalar(uLeft), firstRow);
        cv::subtract(firstRow, pLeft.row(0), firstRow);

        // the second row
        cv::Mat secondRow = out_uvpMat.row(1);
        cv::multiply(pLeft.row(2), cv::Scalar(vLeft), secondRow);
        cv::subtract(secondRow, pLeft.row(1), secondRow);

        // the third row
        cv::Mat thirdRow = out_uvpMat.row(2);
        cv::multiply(pRight.row(2), cv::Scalar(uRight), thirdRow);
        cv::subtract(thirdRow, pRight.row(0), thirdRow);

        // the forth row
        cv::Mat forthRow = out_uvpMat.row(3);
        cv::multiply(pRight.row(2), cv::Scalar(vRight), forthRow);
        cv::subtract(forthRow, pRight.row(1), forthRow);
    }
}
