#include <iostream>

using namespace std;

//char decitions[100] = {'S','U','L','L','U','L','U','L','L','L','U','L','F'}; // track 1
//char decitions[100] = {'S','U','L','L','L','S','U','L','L','U','S','U','L','L','S','L','U','L','S','L','F'}; // track 2 (Left hand)
// char decitions[100] = {'R','S','U','R','R','S','R','U','R','S','U','R','F'}; // track 2 (Right hand)
// char decitions[100] = {R, R, S, R, U, R, S, U, R, U, R, S, R, F}
// char decitions[100] = {'R','R','U','S','U','R','R','S','R','U','R','S','U','R','U','R','S','R','F'}; // track 3 (Right hand)
char decitions[100] = {'L','L','U','S','U','L','L','U','L','U','L','L','F'};
char simplifiedDecitions[100];
int i = 0, j = 0;

void replace(bool first){
    char before = decitions[i-1];
    char after = decitions[i+1];

    if (first == true) {
        before = simplifiedDecitions[j-1];
        cout << "before: " << before << endl;
        j--;
    }

    // Caso específico: LUL
        if (before == 'L' && after == 'L') {
            simplifiedDecitions[j++] = 'S';
            cout << "LUL" << endl;
             
        }
        // Caso específico: LUS
        else if (before == 'L' && after == 'S') {
            simplifiedDecitions[j++] = 'R';
            cout << "LUS" << endl;
            
        }
        // Caso específico: RUR
        else if (before == 'R' && after == 'R') {
            simplifiedDecitions[j++] = 'S';
            cout << "RUR" << endl;
            
        }
        // Caso específico: RUS
        else if (before == 'R' && after == 'S') {
            simplifiedDecitions[j++] = 'L';
            cout << "RUS" << endl;
            
        }
        // Caso específico: SUL
        else if (before == 'S' && after == 'L') {
            simplifiedDecitions[j++] = 'R';
            cout << "SUL" << endl;
            
        }
        // Caso específico: SUR
        else if (before == 'S' && after == 'R') {
            simplifiedDecitions[j++] = 'L';
            cout << "SUR" << endl;
            
        }
        // Caso específico: SUS
        else if (before == 'S' && after == 'S') {
            simplifiedDecitions[j++] = 'U';
            cout << "SUS" << endl;
            
        }
        // Caso específico: LUR
        else if (before == 'L' && after == 'R') {
            simplifiedDecitions[j++] = 'U';
            cout << "LUR" << endl;
            
        }
        // Caso específico: RUL
        else if (before == 'R' && after == 'L') {
            simplifiedDecitions[j++] = 'U';
            cout << "RUL" << endl;
            
        }
}

void replaceExeption(){
    j--;
    char before = simplifiedDecitions[j-1];
    char after = decitions[i];

    // Caso específico: LUL
    if (before == 'L' && after == 'L') {
        simplifiedDecitions[j-1] = 'S';
        cout << "LUL" << endl;
            
    }
    // Caso específico: LUS
    else if (before == 'L' && after == 'S') {
        simplifiedDecitions[j-1] = 'R';
        cout << "LUS" << endl;
        
    }
    // Caso específico: RUR
    else if (before == 'R' && after == 'R') {
        simplifiedDecitions[j-1] = 'S';
        cout << "RUR" << endl;
        
    }
    // Caso específico: RUS
    else if (before == 'R' && after == 'S') {
        simplifiedDecitions[j-1] = 'L';
        cout << "RUS" << endl;
        
    }
    // Caso específico: SUL
    else if (before == 'S' && after == 'L') {
        simplifiedDecitions[j-1] = 'R';
        cout << "SUL" << endl;
        
    }
    // Caso específico: SUR
    else if (before == 'S' && after == 'R') {
        simplifiedDecitions[j-1] = 'L';
        cout << "SUR" << endl;
        
    }
    // Caso específico: SUS
    else if (before == 'S' && after == 'S') {
        simplifiedDecitions[j-1] = 'U';
        cout << "SUS" << endl;
        
    }
    // Caso específico: LUR
    else if (before == 'L' && after == 'R') {
        simplifiedDecitions[j-1] = 'U';
        cout << "LUR" << endl;
        
    }
    // Caso específico: RUL
    else if (before == 'R' && after == 'L') {
        simplifiedDecitions[j-1] = 'U';
        cout << "RUL" << endl;
        
    }
    simplifiedDecitions[j] = '0'; 
    cout << "Simplified: " << simplifiedDecitions << endl;
    i++;

}

bool simplify() {
    bool firstComp = true; // Flag first comparison
    bool exeption = false; // Flag for exceptions (RUL, LUR)

    bool finished = false;
    if (decitions[i] == 'F') {
        cout << "Finished" << endl;
        finished = true;
        return finished;
    }
    
    // Iterate over the decitions array
    while (decitions[i] != 'U' && decitions[i] != 'F') {

        if ((simplifiedDecitions[j-1] == 'U')&&((j-1) != 0)) {
            cout << "U found" << endl;
            exeption = true;
            break;
        }
        
        if (decitions[i+1] != 'U' && decitions[i] != 'U')
            simplifiedDecitions[j++] = decitions[i];

        i++;
        
        cout << "Simplified: " << simplifiedDecitions << endl;
        firstComp = false;
    }

    if(exeption == true){
        replaceExeption();  
        return finished;
    }

    if ((decitions[i] == 'U')) {
        replace(firstComp);
        // Jump after comparison
        i += 2; 
    } 
    cout << "-----------------" << endl;
    cout << "i: " << i << endl;
    cout << "Simplified: " << simplifiedDecitions << endl;

    return finished;
}

int main() {
    bool finished = false;
    /*
    bool first = true;
    if(first == true && decitions[i] != 'U'){
        simplifiedDecitions[j++] = decitions[i];
        first = false;
        i++;
    }
    */
    while (finished == false) {
        finished = simplify();
    }
    cout << "Simplified: " << simplifiedDecitions << endl;
    return 0;
}