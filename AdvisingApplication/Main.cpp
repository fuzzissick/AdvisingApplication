#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"
#include "AdvisingDBAbstraction.h"
#include "Main.h"


using namespace std;

int main()
{
	cout.width(6);
	cout << "-------OPENING DATABASE-------" << '\n';
	AdvisingDBAbstraction db("C:\\Users\\Quade-Laptop\\Documents\\AdvisingTest\\test.sqlite");

	int answer = 0;

	while (answer != -1) {
		cout << "-- OPTIONS -- " << endl;
		cout << "1: Insert Student" << endl;
		cout << "2: Insert Advisor" << endl;
		cout << "3: Insert Course w/ Prereqs" << endl;
		cout << "4: Create Plan" << endl;
		cout << "5: Plan Manager" << endl;
		cout << "8: Populate Database" << endl;
		cout << "9: Print History" << endl;
		cout << "-1: Quit" << endl;

		cout << endl << "Option: "; cin >> answer;

		switch (answer)
		{
		case 1: {
			system("cls");
			//Initializers
			string firstName;
			string lastName;
			int AdvisorID;
			
			//form submission
			cout << "Please Enter Student First Name: "; cin >> firstName;
			cout << "Please Enter Student Last Name: "; cin >> lastName;
			system("cls");

			//Print Advisors to choose from
			cout << "--- ADVISORID & NAME ---" << endl;
			db.printAdvisors();
			cout << endl;
			cout << "Please Enter AdvisorID: "; cin >> AdvisorID;

			//Insert
			db.insertStudent(firstName, lastName, AdvisorID);

			system("cls");

			//Add Event to History
			string evnt = "Added Student " + firstName + " " + lastName;
			db.addEvent(evnt);
			cout << "Successfully " << evnt << endl << endl;

			break;
		}
		case 2: {
			system("cls");

			//Initializers
			string firstName;
			string lastName;
			int AdvisorID;

			//Form Submission
			cout << "Please Enter Advisor First Name: "; cin >> firstName;
			cout << "Please Enter Advisor Last Name: "; cin >> lastName;

			//Insert
			db.insertAdvisor(firstName, lastName);
			system("cls");

			//Add Event to History
			string evnt = "Added Advisor " + firstName + " " + lastName;
			db.addEvent(evnt);
			cout << "Successfully " << evnt << endl << endl;
			break;
		}
		case 3: {
			system("cls");

			//Initializers
			int number;
			string CourseName;
			int credits;
			bool required;
			vector<int> prereqs = vector<int>();

			int addPrereq = 0;

			//Form Submission
			cout << "Course Number: "; cin >> number;
			cout << "Course Name: "; cin >> CourseName;
			cout << "Credits Worth: "; cin >> credits;
			cout << "Is it required?(0 for no, 1 for yes) "; cin >> required;

			//Choose PreReqs
			system("cls");
			//Print and choose prereqs
			while (addPrereq != -1)
			{
				db.printCourses();
				cout << endl;
				cout << "Please choose a prereq to add or -1 to stop adding: "; cin >> addPrereq;
				if (addPrereq != -1)
				{
					prereqs.push_back(addPrereq);
				}
			}
			db.insertCourse(number, CourseName, credits, required, prereqs);
			system("cls");

			//Add Event to History
			string evnt = "Added Course " + CourseName;
			db.addEvent(evnt);
			cout << "Successfully " << evnt << endl << endl;
			break;
		}
		case 4: {
			system("cls");
			//Initializers
			db.makePlan();
			string evnt = "Added Plan";
			db.addEvent(evnt);
			cout << "Successfully " << evnt << endl << endl;
			break;
		}
		case 5: {
			system("cls");
			//Initializers
			db.PlanManager();
			break;
		}
		case 8: {
			db.populateDatabase();
			string evnt = "Populated Database";
			db.addEvent(evnt);
			cout << "Successfully " << evnt << endl << endl;
			break;
		}
		case 9:
			db.printHistory();
			break;
		case -1:
			cout << "Thank you for using my app. Take a look at my beautiful source code (:" << endl;
			system("pause");
			exit(1);
		default:
			cout << "Please choose a correct option" << endl;
			break;
		}

	}

	system("pause");
	return 0;

}