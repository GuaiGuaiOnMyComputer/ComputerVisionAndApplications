#include <array>
#include <execution>
#include <forward_list>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "directtriangulation.hpp"
#include "cvmatdebug.hpp"

namespace finprj
{
    DirectTriangulation::DirectTriangulation(
        const std::array<double, 3 * 4> &leftRt, const std::array<double, 3 * 4> &rightRt, 
        const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK) noexcept : 
            _leftRt{cv::Mat(3, 4, CV_64FC1, (void*)leftRt.data())}, _rightRt{cv::Mat(3, 4, CV_64FC1, (void*)rightRt.data())},
            _leftK{cv::Mat(3, 3, CV_64FC1, (void*)leftK.data())}, _rightK{cv::Mat(3, 3, CV_64FC1, (void*)rightK.data())}
    {
        _leftP = _leftK * _leftRt;
        _leftP /= _leftP.at<double>(2, 3);
        _rightP = _rightK * _rightRt;
        _rightP /= _rightP.at<double>(2, 3);

    }

    DirectTriangulation::DirectTriangulation(const std::array<double, 4 * 3> &leftP, const std::array<double, 4 * 3> &rightP) noexcept :
        _leftRt{cv::Mat()}, _rightRt{cv::Mat()},
        _leftK{cv::Mat()}, _rightK{cv::Mat()},
        _leftP{cv::Mat(3, 4, CV_64FC1, (void*)leftP.data())}, _rightP{cv::Mat(3, 4, CV_64FC1, (void*)rightP.data())}
    {
        _leftP = _leftK * _leftRt;
        _leftP /= _leftP.at<double>(2, 3);
        _rightP = _rightK * _rightRt;
        _rightP /= _rightP.at<double>(2, 3);
    }

    void DirectTriangulation::FilterOutOfBoundPoints(const std::vector<cv::Point3d> &projectedWorldPoints, const cv::Range& xRange, const cv::Range& yRange, const cv::Range& zRange, std::list<const cv::Point3d *> &in_out_validWorldPoints)
    {
        for (const cv::Point3d& worldPt : projectedWorldPoints)
        {
            const bool xWithinRangeFlag = (worldPt.x > xRange.start) && (worldPt.x < xRange.end);
            const bool yWithinRangeFlag = (worldPt.y > yRange.start) && (worldPt.y < yRange.end);
            const bool zWithinRangeFlag = (worldPt.z > zRange.start) && (worldPt.z < zRange.end); ;
            if (xWithinRangeFlag && yWithinRangeFlag && zWithinRangeFlag)
                in_out_validWorldPoints.push_front(&worldPt);
        }
    }

    void DirectTriangulation::FilterOutliners(const cv::Mat &rightCameraP, const cv::Size &rightImageSize, std::list<const cv::Point3d *> &in_out_validWorldPoint_ptrs)
    {
        auto in_out_validWorldPoint_ptrs_iter = in_out_validWorldPoint_ptrs.begin();
        while (in_out_validWorldPoint_ptrs_iter != in_out_validWorldPoint_ptrs.end())
        {
            const cv::Vec4d worldPtHomo((*in_out_validWorldPoint_ptrs_iter)->x, (*in_out_validWorldPoint_ptrs_iter)->y, (*in_out_validWorldPoint_ptrs_iter)->z, 1);
            std::array<double, 3> localPointBuffer_;
            cv::Mat1d localPoint(3, 1, localPointBuffer_.data());
            localPoint = rightCameraP * worldPtHomo;
            localPoint /= localPoint(3, 0);

            if (localPoint(0, 0) < 0 || localPoint(1, 0) < 0)
                in_out_validWorldPoint_ptrs.erase(in_out_validWorldPoint_ptrs_iter++);
            if (localPoint(0, 0) > rightImageSize.width || localPoint(1, 0) > rightImageSize.height)
                in_out_validWorldPoint_ptrs.erase(in_out_validWorldPoint_ptrs_iter++);
        }
    }



    cv::Point2d DirectTriangulation::WorldToLocal(const cv::Mat &cameraP, const cv::Point3d& worldPoint)
    {
        const cv::Vec4d worldPointHomo(worldPoint.x, worldPoint.y, worldPoint.z, 1);
        std::array<double, 3> localPointBuffer_;
        cv::Mat1d localPoint(3, 1, localPointBuffer_.data());
        localPoint = cameraP * worldPointHomo;
        return cv::Point(
            localPoint(0, 0) / localPoint(2, 0),
            localPoint(1, 0) / localPoint(2, 0)
        );
    }

    const cv::Mat& DirectTriangulation::GetRightP() const 
    {
        return this->_rightP;
    }

    std::vector<cv::Point3d> DirectTriangulation::LocalToWorld(const std::vector<cv::Point2d> &pointsLeft, const std::vector<cv::Point2d> &pointsRight)
    {
        std::vector<cv::Point3d> worldPoints(pointsLeft.size());
        std::transform(
            std::execution::par, 
            pointsLeft.cbegin(),
            pointsLeft.cend(),
            pointsRight.cbegin(),
            worldPoints.begin(),
            [&](const cv::Point2d &pLeft, const cv::Point2d &pRight) -> cv::Point3d
                { return DirectTriangulation::LocalToWorld(pLeft, pRight); });
        return worldPoints;
    }

    std::vector<cv::Point3d> DirectTriangulation::LocalToWorld(const cv::Mat_<cv::Point> &pointsLeft, const cv::Mat_<cv::Point> &pointsRight)
    {
        std::vector<cv::Point3d> worldPoints(pointsLeft.total());
        std::transform(
            std::execution::par, 
            pointsLeft.begin(),
            pointsLeft.end(),
            pointsRight.begin(),
            worldPoints.begin(),
            [&](const cv::Point2d &pLeft, const cv::Point2d &pRight) -> cv::Point3d
                { return DirectTriangulation::LocalToWorld(pLeft, pRight); });
        return worldPoints;
    }

    std::vector<cv::Point3d> DirectTriangulation::LocalToWorld(const std::list<const cv::Point*>& pointsLeft, const std::list<const cv::Point*>& pointsRight, const size_t pointCount)
    {
        std::vector<cv::Point3d> worldPoints(pointCount);
        std::transform(
            std::execution::par, 
            pointsLeft.cbegin(),
            pointsLeft.cend(),
            pointsRight.cbegin(),
            worldPoints.begin(),
            [&](const cv::Point* ptLeft_ptr, const cv::Point* ptRight_ptr) -> cv::Point3d
                { return DirectTriangulation::LocalToWorld(cv::Point2d(ptLeft_ptr->x, ptRight_ptr->y), cv::Point2d(ptRight_ptr->x, ptRight_ptr->y)); });
        return worldPoints;
    }

    cv::Point3d DirectTriangulation::LocalToWorld(const cv::Point2d& pointLeft, const cv::Point2d& pointRight)
    {
        std::array<double, 4 * 4> uvpMatBuffer;
        cv::Mat_<double> uvpMat(4, 4, uvpMatBuffer.data());
        DirectTriangulation::_get_uvp_mat(pointLeft, pointRight, this->_leftP, this->_rightP, uvpMat);

        std::array<double, 4 * 4> leftEigenVectorMatrixBuffer_;
        cv::Mat_<double> leftEigenVectorMatrix(4, 4, leftEigenVectorMatrixBuffer_.data());

        std::array<double, 4 * 4> rightEigenVectorMatrixBufferT_;
        cv::Mat_<double> rightEigenVectorMatrixT(4, 4, rightEigenVectorMatrixBufferT_.data());

        std::array<double, 4> eigenValuesBuffer_;
        cv::Mat_<double> eigenValues(4, 1, eigenValuesBuffer_.data());

        cv::SVD::compute(uvpMat, eigenValues, leftEigenVectorMatrix, rightEigenVectorMatrixT);
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
        constexpr double w = 1.0;
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
