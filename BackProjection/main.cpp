// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int cameraNum;
int cameraCount = 0;
int lineCount = 1;

const double imageCentreX = 540.0;
const double imageCentreY = 960.0;

struct cameraInfo {
    double focalLen;
    double K [3][3];//IntrinsicMatrix
    double T [1][3];//Translation
    double C [1][3];//CameraPosition
    double aR [1][3];//AxisAngleR
    double qR [1][4];//QuaternionR
    double R [3][3];//3x3R
    double rD;//RadialDistortion
    double P [3][4];//CameraMatrix
};

struct point2D {
    double x;
    double y;
};

cameraInfo camera [200];
point2D cameraPoint [200][200];

void initialiseK(){
    for (int i; i <200; i++) {
            camera[i].K [0][0] = 0.0;
            camera[i].K [0][1] = 0.0;
            camera[i].K [0][2] = imageCentreX;
            camera[i].K [1][0] = 0.0;
            camera[i].K [1][1] = 0.0;
            camera[i].K [1][2] = imageCentreY;
            camera[i].K [2][0] = 0.0;
            camera[i].K [2][1] = 0.0;
            camera[i].K [2][2] = 1.0;
        };
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
                camera[cameraCount - 1].K [0][0] = camera[cameraCount - 1].focalLen;
                camera[cameraCount - 1].K [1][1] = camera[cameraCount - 1].focalLen;
                //cout << "FocalLen: " << focalLen [cameraCount - 1] << "\n";
            }
            else if (lineCount == 4) {
                //storing Translation T
                string x, y, z;
                myfile >> x >> y >> z;
                camera[cameraCount - 1].T [0][0] = stringToDouble(x);
                camera[cameraCount - 1].T [0][1] = stringToDouble(y);
                camera[cameraCount - 1].T [0][2] = stringToDouble(z);
                //cout << "T: " << T [cameraCount - 1][0][0] << " " << T [cameraCount - 1][0][1] << " " << T [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 5) {
                //storing Camera position C
                string x, y, z;
                myfile >> x >> y >> z;
                C [cameraCount - 1][0][0] = stringToDouble(x);
                C [cameraCount - 1][0][1] = stringToDouble(y);
                C [cameraCount - 1][0][2] = stringToDouble(z);
                //cout << "C: " << C [cameraCount - 1][0][0] << " " << C [cameraCount - 1][0][1] << " " << C [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 6) {
                //storing Axis angle format of R
                string x, y, z;
                myfile >> x >> y >> z;
                aR [cameraCount - 1][0][0] = stringToDouble(x);
                aR [cameraCount - 1][0][1] = stringToDouble(y);
                aR [cameraCount - 1][0][2] = stringToDouble(z);
                //cout << "aR: " << aR [cameraCount - 1][0][0] << " " << aR [cameraCount - 1][0][1] << " " << aR [cameraCount - 1][0][2] << "\n";
            }
            else if (lineCount == 7) {
                //storign Quaternion format of R
                string x, y, z, w;
                myfile >> x >> y >> z >> w;
                qR [cameraCount - 1][0][0] = stringToDouble(x);
                qR [cameraCount - 1][0][1] = stringToDouble(y);
                qR [cameraCount - 1][0][2] = stringToDouble(z);
                qR [cameraCount - 1][0][3] = stringToDouble(w);
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
                R [cameraCount - 1][row][0] = stringToDouble(x);
                R [cameraCount - 1][row][1] = stringToDouble(y);
                R [cameraCount - 1][row][2] = stringToDouble(z);
                //cout << "R: " << R [cameraCount - 1][row][0] << " " << R [cameraCount - 1][row][1] << " " << R [cameraCount - 1][row][2] << "\n";
            }
            else if (lineCount == 12) {
                //storing Normalized radial distortion
                stringstream convert(line);
                convert >> rD [cameraCount - 1];
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
        cout << "\n";
        if (pfile.is_open()) {
            while (getline(pfile, line)) {
                if (lineCount < 3) {
                    string x, y, z, w;
                    pfile >> x >> y >> z >> w;
                    //cout << "\n" << lineCount << "\n";
                    P [currentCamera][lineCount - 1][0] = stringToDouble(x);
                    P [currentCamera][lineCount - 1][1] = stringToDouble(y);
                    P [currentCamera][lineCount - 1][2] = stringToDouble(z);
                    P [currentCamera][lineCount - 1][3] = stringToDouble(w);
                    cout << P [currentCamera][lineCount - 1][0] << " " << P [currentCamera][lineCount - 1][1] << " " << P [currentCamera][lineCount - 1][2]<< " " << P [currentCamera][lineCount - 1][3] << "\n";
                }
                lineCount++;
            }
            if (pfile.eof()) {
                pfile.close();
            }
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
