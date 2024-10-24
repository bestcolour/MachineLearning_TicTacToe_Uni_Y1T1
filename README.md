# MachineLearning_TicTacToe_Uni_Y1T1
 Machine Learning Tic Tac Toe game project done in university's year 1 trimester 1



1.	Install the ‘raylib’ GUI library at https://www.raylib.com/ keep all installation settings default.
 
2.	After successful installation, locate where your raylib folder is installed at.
3.	To compile the .c source code to output the .exe file, run the following command. The text in red is the respective file path to the installed raylib folder. Depending on where you have installed it, you will have to make changes accordingly.

gcc -o tic_tac_toe_integrated tic_tac_toe_integrated.c -I"C:\raylib\w64devkit\x86_64-w64-mingw32\include" -L"C:\raylib\w64devkit\x86_64-w64-mingw32\lib" -lraylib -lopengl32 -lgdi32 -lwinmm

4.	After successful compilation, run the following command to launch the program.

.\tic_tac_toe_integrated
