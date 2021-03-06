#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/PointStamped.h"
#include "sensor_msgs/PointCloud.h"
#include "visualization_msgs/Marker.h"
#include "localization/InitLocalization.h"
#include "localization/IOFromBoard.h"
#include "localization/beach_map.h"
#include "tf/transform_datatypes.h"
#include "tf/transform_broadcaster.h"
#include "tf/transform_listener.h"
#include "pole.cpp"
#include <Eigen/Dense>
#include <cmath>

class Loc {
 public:
	Loc();

 private:
	ros::NodeHandle n_;
	ros::Subscriber sub_scan_;
	ros::Subscriber sub_odom_;
	ros::Subscriber sub_imu_;
	ros::ServiceServer srv_init_;
	ros::Publisher pub_pose_;
	ros::Publisher pub_pole_;
	ros::Publisher pub_map_;
	ros::Publisher pub_marker_;
	ros::Publisher pub_cloud_;

	double b;	//wheel distance of robot
	double pole_radius;	//radius of reflective poles
	double laser_height_;
	bool use_odometry_;	//if using pioneer for testing
	sensor_msgs::LaserScan scan_;
	sensor_msgs::PointCloud cloud_;
	localization::IOFromBoard odom_;
	localization::IOFromBoard last_odom_;
	std::vector<Pole> poles_;
	geometry_msgs::PoseWithCovarianceStamped pose_;
	geometry_msgs::PoseWithCovarianceStamped last_pose_;
	geometry_msgs::PoseStamped initial_pose_;
	geometry_msgs::Pose pred_pose_;
	sensor_msgs::Imu last_attitude_;
	sensor_msgs::Imu attitude_;
	bool initiation_;
	ros::Time current_time_;
	double scan_covariance_;
	double k_s_;
	double k_th_;
	double laser_offset_;
	tf::TransformListener listener_;

	void NormalizeAngle(double& angle);
	void StateHandler();
	void InitiatePoles();
	void PublishPoles();
	void PublishPose();
	void PublishMap();
	void PublishTf();
	void PublishCloud(const sensor_msgs::PointCloud &cloud);
	void Locate();
	void RefreshData();
	void UpdatePoles(const std::vector<Eigen::Vector3d> &scans_to_sort);
	void GetPose();
	void EstimateInvisiblePoles();
	void PrintPose();
	void CalcPose(const Pole &pole1, const Pole &pole2, std::vector<geometry_msgs::Pose> *pose_vector);
	bool IsPolePoint(const double &intensity, const double &distance);
	void MinimizeScans(std::vector<Eigen::Vector3d> *scan);
	void CorrectMoveError(std::vector<Eigen::Vector3d> *scan_pole_points);
	void ScanToCloud();
	void ScanCallback(const sensor_msgs::LaserScan &scan);
	void OdomCallback(const localization::IOFromBoard &odom);
	bool InitService(localization::InitLocalization::Request &req, localization::InitLocalization::Response &res);
	void ImuCallback(const sensor_msgs::Imu &attitude);
	void SetInit(const bool &init);
	//Kalman functions
	void DoTheKalman();
	void SetTime();
	Eigen::Matrix3d StateJacobi(const double &ds, const double &dth, const double &theta);
	Eigen::MatrixXd InputJacobi(const double &ds, const double &dth, const double &theta);
	Eigen::Matrix2d Q(const double &ds, const double &dth);
	Eigen::VectorXd EstimateReferencePoint(const std::vector<Pole> &visible_poles, const Eigen::Vector3d &state);
	Eigen::MatrixXd EstimateJacobi(const std::vector<Pole> &visible_poles, const Eigen::Vector3d &state);
	Eigen::MatrixXd ErrorMatrix(const std::vector<Pole> &visible_poles, const Eigen::Vector3d &state);
	Eigen::VectorXd CalculateMeasuredPoints(const std::vector<Pole> &visible_poles);
};
