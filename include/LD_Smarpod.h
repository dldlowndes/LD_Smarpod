#ifndef LD_SMARPOD_H
#define LD_SMARPOD_H

#include <SmarPod.h>

#include <string>
#include <vector>

class LD_Smarpod
{
    public:
        LD_Smarpod();
        virtual ~LD_Smarpod();
        LD_Smarpod(const LD_Smarpod& other);

        int Find_Systems();

        bool do_Calibrate;
        int enableAccelerationControl;

        unsigned int m_model;
        std::string m_mcsLocator;

        double xyMax;
        Smarpod_Pose pZero;

        Smarpod_Status sp_Ret;
        unsigned int major, minor, update;
        int is_Referenced;
        unsigned int id;
        unsigned int status;
        Smarpod_Pose pose;

        int SetSpeed(double new_Speed); // Page 39/40
        int SetAcceleration(double new_Acceleration); // Page 41/42
        int SetPivot(std::vector<double> new_Pivot); // Page 43/44

        int Move_Absolute(Smarpod_Pose new_Position); // Page 48
        int Move_Relative(Smarpod_Pose move_Amount); // Page 48
        int Move_Origin();
        int Move_Stop(bool and_Hold); //Page 49/50

        Smarpod_Pose Get_Current_Pose(); // Page 46
        int Is_Moving(); // Page 47
        int Is_Holding(); // Page 47

    private:
        int Init(unsigned int model, std::string mcsLocator);
        int Close();
};

std::vector<Smarpod_Pose> MonoScanX(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize);

std::vector<Smarpod_Pose> MonoScanY(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize);

std::vector<Smarpod_Pose> BiScanX(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize);

std::vector<Smarpod_Pose> BiScanY(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize);

std::vector<Smarpod_Pose> SpiralScan(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize);

int Poses_To_File(std::vector<Smarpod_Pose> poses, std::string filename);

#endif // LD_SMARPOD_H
