// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <armadillo>

using namespace arma;
using namespace std;

int cameraNum;
int cameraCount = 0;
int lineCount = 1;

mat r1;
mat r2;
mat r3;

const double imageCentreX = 540.0;
const double imageCentreY = 960.0;

struct point2D {
    double x;
    double y;
};

struct cameraInfo {
    double focalLen;
    mat K;//IntrinsicMatrix 3x3
    vec T;//Translation 3x1
    vec C;//CameraPosition 3x1
    vec aR;//AxisAngleR 3x1
    vec qR;//QuaternionR 4x1
    mat R;//3x3R
    double rD;//RadialDistortion
    mat P;//CameraMatrix
    
    point2D cameraPoint [200];//2D points
};

cameraInfo camera [200];

void initialiseK(){
    for (int i; i <200; i++) {
        camera[i].K << 0 << 0 << imageCentreX << endr
                    << 0 << 0 << imageCentreY << endr
                    << 0 << 0 << 1;
    }
}

double stringToDouble(string s) {
    double d = atof(s.c_str());
    return d;
}

void readFile() {
    std::string line;
    ifstream camfile ("cameras_v2.txt");
    string RMatrix;
    if (camfile.is_open()) {
        while (getline(camfile, line)) {
            if (lineCount == 3) {
                //storing focal length
                cameraCount++;
                //cout << "\n" << "Storing camera " << cameraCount << "\n";
                camera[cameraCount - 1].focalLen = stringToDouble(line);
                camera[cameraCount - 1].K(1,1) = camera[cameraCount - 1].focalLen;
                camera[cameraCount - 1].K(2,2) = camera[cameraCount - 1].focalLen;
                //cout << "FocalLen: " << focalLen [cameraCount - 1] << "\n";
            }
            else if (lineCount == 4) {
                //storing Translation T
                double x, y, z;
                camfile >> x >> y >> z;
                camera[cameraCount - 1].T << x << endr << y << endr << z;
                //cout << "T: " << T [cameraCount - 1][0][0] << " " << T [cameraCount - 1][0][1] << " " << T [cameraCount - 1][0][2] << "\n";
                //cout << camera[cameraCount - 1].T << "\n";
            }
            else if (lineCount == 5) {
                //storing Camera position C
                double x1, y1, z1;
                camfile >> x1 >> y1 >> z1;
                camera[cameraCount - 1].C << x1 << endr << y1 << endr << z1;
                //cout << "C: " << C [cameraCount - 1][0][0] << " " << C [cameraCount - 1][0][1] << " " << C [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 6) {
                //storing Axis angle format of R
                double x, y, z;
                camfile >> x >> y >> z;
                camera[cameraCount - 1].aR << x << endr << y << endr << z;
                //cout << "aR: " << aR [cameraCount - 1][0][0] << " " << aR [cameraCount - 1][0][1] << " " << aR [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 7) {
                //storign Quaternion format of R
                double x, y, z, t;
                camfile >> x >> y >> z >> t;
                camera[cameraCount - 1].qR << x << endr << y << endr << z << endr << t;
                //cout << "qR: " << qR [cameraCount - 1][0][0] << " " << qR [cameraCount - 1][0][1] << " " << qR [cameraCount - 1][0][2] << " " << qR [cameraCount - 1][0][3] << "\n";
            }
            else if (lineCount == 8) {
                //storing Matrix format of R
                double x1, x2, x3;
                camfile >> x1 >> x2 >> x3;
                r1 << x1 << x2 << x3;
            }
            else if (lineCount == 9) {
                double y1, y2, y3;
                camfile >> y1 >> y2 >> y3;
                r2 << y1 << y2 << y3;
                r3 = join_cols(r1, r2);
            }
            else if (lineCount == 10) {
                double z1, z2, z3;
                camfile >> z1 >> z2 >> z3;
                r2 << z1 << z2 << z3;
                camera[cameraCount - 1].R = join_cols(r3, r2);
            }
            else if (lineCount == 12) {
                //storing Normalized radial distortion
                stringstream convert(line);
                convert >> camera[cameraCount - 1].rD;
                //cout << "rD: " << rD [cameraCount - 1] << "\n";
            }
            if (lineCount > 14) {
                lineCount = 1;
            }
            lineCount++;
            if (camfile.eof()) {
                camfile.close();
            }
        }
    }
    else cout << "Unable to open file";
}

string getFileName(int cameraNum) {
    stringstream convert;
    convert << cameraNum;
    string fileName = convert.str();
    fileName = fileName + ".txt";
    while (fileName.length() < 12) {
        fileName = '0' + fileName;
    }
    return fileName;
}

void readPFiles() {
    for (int currentCamera = 0; currentCamera < cameraCount; currentCamera++) {
        std::string line;
        ifstream pfile (getFileName(currentCamera));
        if (pfile.is_open()) {
            while (pfile >> line) {
                    double x1, x2, x3, x4,
                           y1, y2, y3, y4,
                           z1, z2, z3, z4;
                    
                    pfile >> x1 >> x2 >> x3 >> x4
                          >> y1 >> y2 >> y3 >> y4
                          >> z1 >> z2 >> z3 >> z4;
                    //cout << "\n" << lineCount << "\n";
                    
                    camera[currentCamera].P << x1 << x2 << x3 << x4 << endr
                                            << y1 << y2 << y3 << y4 << endr
                                            << z1 << z2 << z3 << z4;
            }
            pfile.close();
            //cout << camera[currentCamera].P << "\n";
        }
        else cout << "Unable to open file";
    }
}

void calculate2DPoint(){
    //calculate points
}


int main () {
    initialiseK();
    readFile();
    readPFiles();
    //cout << camera[0].R << "\n";
    calculate2DPoint();
    
    return 0;
}
