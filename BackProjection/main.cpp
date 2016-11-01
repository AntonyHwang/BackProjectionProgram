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

const double imageCentreX = 540.0;
const double imageCentreY = 960.0;

struct point2D {
    double x;
    double y;
};

struct cameraInfo {
    double focalLen;
    mat K;//IntrinsicMatrix
    double T[1][3];//Translation
    double C[1][3];//CameraPosition
    double aR[1][3];//AxisAngleR
    double qR[1][4];//QuaternionR
    double R[3][3];//3x3R
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
    ifstream myfile ("cameras_v2.txt");
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
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
                string x, y, z;
                myfile >> x >> y >> z;
                camera[cameraCount - 1].T[0][0] = stringToDouble(x);
                camera[cameraCount - 1].T[0][1] = stringToDouble(y);
                camera[cameraCount - 1].T[0][2] = stringToDouble(z);
                //cout << "T: " << T [cameraCount - 1][0][0] << " " << T [cameraCount - 1][0][1] << " " << T [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 5) {
                //storing Camera position C
                string x, y, z;
                myfile >> x >> y >> z;
                camera[cameraCount - 1].C[0][0] = stringToDouble(x);
                camera[cameraCount - 1].C[0][1] = stringToDouble(y);
                camera[cameraCount - 1].C[0][2] = stringToDouble(z);
                //cout << "C: " << C [cameraCount - 1][0][0] << " " << C [cameraCount - 1][0][1] << " " << C [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 6) {
                //storing Axis angle format of R
                string x, y, z;
                myfile >> x >> y >> z;
                camera[cameraCount - 1].aR[0][0] = stringToDouble(x);
                camera[cameraCount - 1].aR[0][1] = stringToDouble(y);
                camera[cameraCount - 1].aR[0][2] = stringToDouble(z);
                //cout << "aR: " << aR [cameraCount - 1][0][0] << " " << aR [cameraCount - 1][0][1] << " " << aR [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 7) {
                //storign Quaternion format of R
                string x, y, z, w;
                myfile >> x >> y >> z >> w;
                camera[cameraCount - 1].qR[0][0] = stringToDouble(x);
                camera[cameraCount - 1].qR[0][1] = stringToDouble(y);
                camera[cameraCount - 1].qR[0][2] = stringToDouble(z);
                camera[cameraCount - 1].qR[0][3] = stringToDouble(w);
                //cout << "qR: " << qR [cameraCount - 1][0][0] << " " << qR [cameraCount - 1][0][1] << " " << qR [cameraCount - 1][0][2] << " " << qR [cameraCount - 1][0][3] << "\n";
            }
            else if (lineCount == 8 || lineCount == 9 || lineCount == 10) {
                //storing Matrix format of R
                int row;
                if (lineCount == 8) {
                    row = 0;
                }
                else if ( lineCount == 9) {
                    row = 1;
                }
                else {
                    row = 2;
                }
                string x, y, z;
                myfile >> x >> y >> z;
                camera[cameraCount - 1].R[row][0] = stringToDouble(x);
                camera[cameraCount - 1].R[row][1] = stringToDouble(y);
                camera[cameraCount - 1].R[row][2] = stringToDouble(z);
                //cout << "R: " << R [cameraCount - 1][row][0] << " " << R [cameraCount - 1][row][1] << " " << R [cameraCount - 1][row][2] << "\n";
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
            if (myfile.eof()) {
                myfile.close();
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
        lineCount = 0;
        std::string line;
        ifstream pfile (getFileName(currentCamera));
        //cout << "\n";
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

                    //camera[currentCamera].P[lineCount - 1][0] = stringToDouble(x);
                    //camera[currentCamera].P[lineCount - 1][1] = stringToDouble(y);
                    //camera[currentCamera].P[lineCount - 1][2] = stringToDouble(z);
                    //camera[currentCamera].P[lineCount - 1][3] = stringToDouble(w);
                    //cout << camera[currentCamera].P[lineCount - 1][0] << " " << camera[currentCamera].P[lineCount - 1][1] << " " << camera[currentCamera].P[lineCount - 1][2]<< " " << camera[currentCamera].P[lineCount - 1][3] << "\n";
                    //cout << camera[currentCamera].P << "\n";

                //lineCount++;
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
    calculate2DPoint();
    
    return 0;
}
