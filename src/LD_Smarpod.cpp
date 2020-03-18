#include "LD_Smarpod.h"
#include "LD_Util.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

DaveSmarpod::DaveSmarpod()
{
//    std::cout << "Supported values for model number:" << std::endl;
//    std::vector<unsigned  int> models(128);
//    unsigned int listsize = 128;
//    Smarpod_GetModels(&models.front(), &listsize);
//    for(int i=0; i<listsize; i++){
//        std::cout << "\t" << models[i] << std::endl;
//    }

    // TODO: Don't hard code this.
    // 70.42 is 10003
    //Init(10003, "usb:id:1240801117");
    Init(10003, "usb::ix::0");

}

DaveSmarpod::~DaveSmarpod()
{
    sp_Ret = Close();
    //dtor
}

DaveSmarpod::DaveSmarpod(const DaveSmarpod& other)
{
    //copy ctor
}

int DaveSmarpod::Init(unsigned int model, std::string mcsLocator){
    // Dunno what to do with this but hey.
    Smarpod_GetDLLVersion(&major, &minor, &update);
    printf("using SmarPod library version %u.%u.%u\n",major,minor,update);

    std::cout << "trying to open model: " << model << " locator: " << mcsLocator.c_str() << std::endl;
    sp_Ret = Smarpod_Open(&id, model, mcsLocator.c_str(),"");
    std::cout << "Smarpod_Open says:" << sp_Ret << std::endl;
    // Set up all the things
    // Are there even any sensors on our Smarpod? Let's try and enable anyway.
    // The Windows GUI has them set to "Always On" so why not...
    std::cout << "Smarpod_SetSensorMode says:" << sp_Ret << std::endl;
    sp_Ret = Smarpod_SetSensorMode(id, SMARPOD_SENSORS_ENABLED);
    // Max frequency is 3000 in the manual example. 18500 in Windows GUI program.
    sp_Ret = Smarpod_SetMaxFrequency(id, 18500);
    std::cout << "Smarpod_SetMaxFrequency says:" << sp_Ret << std::endl;
    // Speed/accel in m/s andm/s^2 respectively.
    sp_Ret = Smarpod_SetSpeed(id, 1, 0.01);
    std::cout << "Smarpod_SetSpeed:" << sp_Ret << std::endl;
    sp_Ret = Smarpod_SetAcceleration(id, 1, 0.01);
    std::cout << "Smarpod_SetAcceleration:" << sp_Ret << std::endl;


    // Often the Smarpod remains references between uses. Finding the reference
    // marks can be a pain (the stage moves all over the place!). Check if it
    // needs to be references before performing the step.
    do {
        sp_Ret = Smarpod_IsReferenced(id, &is_Referenced);
        std::cout << "Smarpod_IsReferenced says:" << sp_Ret << std::endl;
        std::cout << "Is Referenced?" << is_Referenced << std::endl;
        if (is_Referenced == false){
            std::cout << "SMARPOD IS NOT REFERENCED. ABOUT TO REFERENCE." << std::endl;
            std::cout << "REFERENCING WILL INVOLVE RANDOM ERRATIC MOVEMENTS OVER THE FULL RANGE OF THE SMARPOD" << std::endl;
            std::cout << "IS THIS OK? [y / N]" << std::endl;
            char choice = std::cin.get();
            if ((choice == 'y') | (choice == 'Y')){
                sp_Ret = Smarpod_FindReferenceMarks(id);
            }
            else{
                std::cout << "quitting instead" << std::endl;
                return 1;
            }
            std::cout << "Smarpod_FindReferenceMarks says:" << sp_Ret << std::endl;
            MySleep(500);
        }
    } while (is_Referenced == 0);
    std::cout << "Is Referenced?" << is_Referenced << std::endl;

    return 0;
}

int DaveSmarpod::Close(){
    sp_Ret = Smarpod_Close(id);
    return sp_Ret;
}

int DaveSmarpod::SetSpeed(double new_Speed){
    sp_Ret = Smarpod_SetSpeed(id, 1, new_Speed);
    return sp_Ret;
}

int DaveSmarpod::SetAcceleration(double new_Acceleration){
    sp_Ret = Smarpod_SetAcceleration(id, 1, new_Acceleration);
    return sp_Ret;
}

int DaveSmarpod::SetPivot(std::vector<double> new_Pivot){
    sp_Ret = Smarpod_SetPivot(id, &new_Pivot.front());
    return sp_Ret;
}

int DaveSmarpod::Move_Absolute(Smarpod_Pose new_Position){
    Smarpod_Status status;
    Smarpod_GetMoveStatus(id, &status);
    if ((status == SMARPOD_STOPPED) |
        (status == SMARPOD_HOLDING)){
        Smarpod_Move(id, &new_Position, SMARPOD_HOLDTIME_INFINITE, 1);
    }
    else{
        // TODO: add an option to keep trying a move rather than just
        // whining and falling over.
        return SMARPOD_BUSY_ERROR;
    }
    return sp_Ret;
}

int DaveSmarpod::Move_Relative(Smarpod_Pose move_Amount){
    Smarpod_Pose pose = Get_Current_Pose();
    pose.positionX += move_Amount.positionX;
    pose.positionY += move_Amount.positionY;
    pose.positionZ += move_Amount.positionZ;
    pose.rotationX += move_Amount.rotationX;
    pose.rotationY += move_Amount.rotationY;
    pose.rotationZ += move_Amount.rotationZ;
    sp_Ret = Move_Absolute(pose);
    return sp_Ret;
}

int DaveSmarpod::Move_Origin(){
    sp_Ret = Move_Absolute(pZero);
    return sp_Ret;
}

int DaveSmarpod::Move_Stop(bool and_Hold){
    if (and_Hold){
        sp_Ret = Smarpod_StopAndHold(id, SMARPOD_HOLDTIME_INFINITE);
    }
    else{
        sp_Ret = Smarpod_Stop(id);
    }
    return sp_Ret;
}

Smarpod_Pose DaveSmarpod::Get_Current_Pose(){
    Smarpod_Pose pose;
    sp_Ret = Smarpod_GetPose(id, &pose);
    return pose;
}

int DaveSmarpod::Is_Moving(){
    Smarpod_Status move_Status;
    sp_Ret = Smarpod_GetMoveStatus(id, &move_Status);
    if (move_Status == SMARPOD_MOVING){
        return 1;
    }
    else{
        return 0;
    }
}

int DaveSmarpod::Is_Holding(){
    Smarpod_Status move_Status;
    sp_Ret = Smarpod_GetMoveStatus(id, &move_Status);
    if (move_Status == SMARPOD_HOLDING){
        return 1;
    }
    else{
        return 0;
    }
}

std::vector<Smarpod_Pose> MonoScanX(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize){
    std::vector<Smarpod_Pose> poses;

    for(double z = z_start; z < z_stop; z+= stepsize){
        for(double x = x_start; x < x_stop; x+= stepsize){
            poses.push_back({x, y_start, z, 0, 0, 0});
        }
    }

    return poses;
}

std::vector<Smarpod_Pose> MonoScanY(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize){
    std::vector<Smarpod_Pose> poses;

    for(double x = x_start; x < x_stop; x+= stepsize){
        for(double z = z_start; z < z_stop; z+= stepsize){
            poses.push_back({x, y_start, z, 0, 0, 0});
        }
    }

    return poses;
}

std::vector<Smarpod_Pose> BiScanX(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize){
    std::vector<Smarpod_Pose> poses;

    bool direction = true;

    for(double z = z_start; z < z_stop; z+= stepsize){
        if (direction){
            for(double x = x_start; x < x_stop; x+= stepsize){
                poses.push_back({x, y_start, z, 0, 0, 0});
            }
        }
        else{
            for(double x = x_stop - stepsize; x > x_start - stepsize; x-= stepsize){
                poses.push_back({x, y_start, z, 0, 0, 0});
            }
        }
        direction = !direction;
    }

    return poses;
}

std::vector<Smarpod_Pose> BiScanY(double x_start, double y_start, double z_start,
                                    double x_stop,  double z_stop, double stepsize){
    std::vector<Smarpod_Pose> poses;

    bool direction = true;

    for(double x = x_start; x < x_stop; x+= stepsize){
        if (direction){
            for(double z = z_start; z < z_stop; z+= stepsize){
                poses.push_back({x, y_start, z, 0, 0, 0});
            }
        }
        else{
            for(double z = z_stop - stepsize; z > z_start - stepsize; z-= stepsize){
                poses.push_back({x, y_start, z, 0, 0, 0});
            }
        }
        direction = !direction;
    }

    return poses;
}



std::vector<Smarpod_Pose> SpiralScan(double x_min, double y_start, double z_min,
                                     double x_max, double z_max,   double stepsize){
    double x_start = (x_max + x_min) / 2;
    double z_start = (z_max + z_min) / 2.0;
    int steps_Max = round((x_max - x_min)/ stepsize) * round((z_max - z_min) / stepsize);

    std::vector<Smarpod_Pose> poses;
    Smarpod_Pose current_Pose = {x_start, y_start, z_start, 0, 0, 0};
    poses.push_back(current_Pose);

    int step_Number = 0;
    bool direction = true;
    int moves_Per_Direction = 1;
    while (step_Number < steps_Max){
        for(int i=0; i < moves_Per_Direction; i++, step_Number++){
            if(direction){
                current_Pose.positionX += stepsize;
            }
            else{
                current_Pose.positionX -= stepsize;
            }
            poses.push_back(current_Pose);
        }
        for(int i=0; i < moves_Per_Direction; i++, step_Number++){
            if(direction){
                current_Pose.positionZ += stepsize;
            }
            else{
                current_Pose.positionZ -= stepsize;
            }
            poses.push_back(current_Pose);
        }
        moves_Per_Direction++;
        direction = !direction;
    }
    return poses;
}

int Poses_To_File(std::vector<Smarpod_Pose> poses, std::string filename){
    std::ofstream file_Out;
    file_Out.open(filename);

    for(unsigned int i=0; i < poses.size(); i++){
        file_Out << poses[i].positionX << ", " <<
                    poses[i].positionY << ", " <<
                    poses[i].positionZ << ", " <<
                    poses[i].rotationX << ", " <<
                    poses[i].rotationY << ", " <<
                    poses[i].rotationZ << std::endl;
    }
    return 0;
}
