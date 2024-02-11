Assignment 1 (Mac/Apple Silicon)

Joshua (Josie) Ng 1005285

int LoadInput(vector<float> &verList, vector<unsigned> &triList)
First I get the name of the inputfile. It is not limited to only the 3 test files.
If the file cannot be open it returns -1 for error.
Else, we iterate through each line, checking for the tokens v, vn and f.
For v and vn, we store them in internal lists, so we can sort them using the face.
For f, we store the unique pairings of each v and vn in an unordered mapping.
We also store the first number (minus 1) of each triplet in triList.
Finally, we populate verList with the vertices in vList, checking for the vnIndex of the first vn in vnList, by retrieving from the v vn mapping.
Since the vList and vnList store 3 coordinatesin flat 1 dimensional lists, we increment by 3 and also only need the first vnIndex.
Finally we close the file as we have loaded the necessary data into memory.

void SetMeshColor(int &colorID)
Since there are 4 colors and we want to iterate throught them, we can incrememnt colorID by 1 and modulo to ensure the number stays within 4.
Then we reassign the colorID.

void RotateModel(float angle, glm::vec3 axis)
First we get an identity matrix, then use the glm rotate function and pass in the matrix, angle and axis, reassignning to transformation matrix.
Finally we do matrix multiplication with the transformation and current model matrix, and reassign the value to current model matrix.

void TranslateModel(glm::vec3 transVec)
First we get an identity matrix, then use the glm translate function and pass in the matrix and translation vector, reassignning to transformation matrix.
Finally we do matrix multiplication with the transformation and current model matrix, and reassign the value to current model matrix.

void ScaleModel(float scale)

First we get an identity matrix, then use the glm scale function and pass in the matrix and a 3 dimensional vector with scale argument for all 3 components as we want to scale equally in all dimensions.
Then we reassign that value to transformation matrix.
Finally we do matrix multiplication with the transformation and current model matrix, and reassign the value to current model matrix.


I also implemented an extra function. When the user presses the R key, it will 'reset' the model view back to the original view when the program was first opened.