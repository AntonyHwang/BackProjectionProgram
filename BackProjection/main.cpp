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

struct point3D {
    double x;
    double y;
    double z;
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
                camera[cameraCount - 1].K(0,0) = camera[cameraCount - 1].focalLen;
                camera[cameraCount - 1].K(1,1) = camera[cameraCount - 1].focalLen;
                //cout << "K: \n" << camera[cameraCount - 1].K << "\n";
            }
            else if (lineCount == 4) {
                //storing Translation T
                double x, y, z;
                camfile >> x >> y >> z;
                camera[cameraCount - 1].T << x << endr << y << endr << z;
                //cout << "T: \n" << camera[cameraCount - 1].T << "\n";
            }
            else if (lineCount == 5) {
                //storing Camera position C
                double x1, y1, z1;
                camfile >> x1 >> y1 >> z1;
                camera[cameraCount - 1].C << x1 << endr << y1 << endr << z1;
                //cout << "C: \n" << camera[cameraCount - 1].C << "\n";
            }
            else if (lineCount == 6) {
                //storing Axis angle format of R
                double x, y, z;
                camfile >> x >> y >> z;
                camera[cameraCount - 1].aR << x << endr << y << endr << z;
                //cout << "aR: \n" << camera[cameraCount - 1].aR << "\n";
            }
            else if (lineCount == 7) {
                //storign Quaternion format of R
                double x, y, z, t;
                camfile >> x >> y >> z >> t;
                camera[cameraCount - 1].qR << x << endr << y << endr << z << endr << t;
                //cout << "qR: \n" << camera[cameraCount - 1].qR << "\n";
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
                //camera[cameraCount - 1].R = join_cols(r3, r2);
                //cout << "R: \n" << camera[cameraCount - 1].R << "\n";
            }
            else if (lineCount == 12) {
                //storing Normalized radial distortion
                stringstream convert(line);
                convert >> camera[cameraCount - 1].rD;
                //cout << "rD: \n" << camera[cameraCount - 1].rD << "\n";
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

string getFileName(string fileType, int index) {
    string fileName;
    if (fileType == "PMatrix") {
        stringstream convert;
        convert << index;
        fileName = convert.str();
        fileName = fileName + ".txt";
        while (fileName.length() < 12) {
            fileName = '0' + fileName;
        }
        return fileName;
    }
    else if (fileType == "Patch") {
        //option-0001.patch
        stringstream convert;
        convert << index;
        fileName = convert.str();
        fileName = fileName + ".patch";
        while (fileName.length() < 10) {
            fileName = '0' + fileName;
        }
        return "option-" + fileName;
    }
    else {
        cout << "cannot find name for this filetype";
        return "file not exist";
    }
}

void readPFiles() {
    for (int currentCamera = 0; currentCamera < cameraCount; currentCamera++) {
        std::string line;
        ifstream pfile (getFileName("PMatrix", currentCamera));
        if (pfile.is_open()) {
            while (pfile >> line) {
                    double x1, x2, x3, x4,
                           y1, y2, y3, y4,
                           z1, z2, z3, z4;
                    
                    pfile >> x1 >> x2 >> x3 >> x4
                          >> y1 >> y2 >> y3 >> y4
                          >> z1 >> z2 >> z3 >> z4;
                    
                    camera[currentCamera].P << x1 << x2 << x3 << x4 << endr
                                            << y1 << y2 << y3 << y4 << endr
                                            << z1 << z2 << z3 << z4;
            }
            pfile.close();
            //cout << "P: \n" << camera[currentCamera].P << "\n";
        }
        else cout << "Unable to open file";
    }
}

void readPatchFile() {
    std::string line;
    std::string word;
    int currentFile = 0;
    string fileName;
    vec point_3D;
    lineCount = 0;
    while (fileName != "file not exist") {
        fileName = getFileName("Patch", currentFile);
        ifstream patchfile (fileName);
        if (patchfile.is_open()) {
            while (getline(patchfile, line)) {
                patchfile >> word;
                if (word == "PATCHS") {
                    lineCount = 2;
                }
                else if (lineCount == 3) {
                    double x, y, z;
                    patchfile >> x >> y >> z;
                    point_3D << stringToDouble(word) << x << y << z;
                    cout << fileName << "\n";
                    cout << point_3D << "\n";
                }
                else if (lineCount == 7) {
                    
                }
                lineCount++;
                if (patchfile.eof()) {
                    patchfile.close();
                    currentFile++;
                }
            }
        }
    }
}


void calculate2DPoint(){
    //calculate points
}


int main () {
    initialiseK();
    readFile();//contains camera information
    readPFiles();//contains the camera matrix of each camera
    readPatchFile();//contains the the 3D coordinates
    //cout << camera[0].R << "\n";
    calculate2DPoint();
    
    return 0;
}
