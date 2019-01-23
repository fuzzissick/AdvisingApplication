#include "AdvisingDBAbstraction.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;



AdvisingDBAbstraction::AdvisingDBAbstraction(string pathToFile)
	: DBAbstraction(pathToFile)
{
	history = vector<string>();
	createAdvisorsTable();
	createStudentsTable();
	createCoursesTable();
	createCoursesTakenTable();
	createCoursePrereqsTable();
	createPlansTable();
	createCoursesInPlanTable();
	createPlanIndex();
}


AdvisingDBAbstraction::~AdvisingDBAbstraction()
{
}


void AdvisingDBAbstraction::createAdvisorsTable()
{

	string sql = "CREATE TABLE IF NOT EXISTS 'Advisors' ( `AdvisorID` INTEGER, `FirstName` TEXT, `LastName` TEXT, PRIMARY KEY(`AdvisorID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Advisors table" << endl;
	}
}


void AdvisingDBAbstraction::createStudentsTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS 'Students' ( `StudentID` INTEGER, `FirstName` TEXT, `LastName` TEXT, `AdvisorID` INTEGER, PRIMARY KEY(`StudentID`), FOREIGN KEY(`AdvisorID`) REFERENCES `Advisors`(`AdvisorID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Students table" << endl;
	}
}


void AdvisingDBAbstraction::createCoursesTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS 'Courses' ( `CourseID` INTEGER, `Number` INTEGER, `CourseName` TEXT, `NumCredits` INTEGER, `Required` INTEGER, PRIMARY KEY(`CourseID`) )";
	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Courses table" << endl;
	}
}


void AdvisingDBAbstraction::createCoursesTakenTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS 'CoursesTaken' ( `CTID` INTEGER, `Semester` TEXT, `Year` INTEGER, `CourseID` INTEGER, `StudentID` INTEGER, PRIMARY KEY(`CTID`), FOREIGN KEY(`CourseID`) REFERENCES `Courses`(`CourseID`), FOREIGN KEY(`StudentID`) REFERENCES 'Students'(`StudentID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Courses Taken table" << endl;
	}
}


void AdvisingDBAbstraction::createCoursePrereqsTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS `CoursePrereqs` ( `CourseID` INTEGER, `PrereqID` INTEGER, FOREIGN KEY(`PrereqID`) REFERENCES `Courses`(`CourseID`), PRIMARY KEY(`CourseID`,`PrereqID`), FOREIGN KEY(`CourseID`) REFERENCES `Courses`(`CourseID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Course Prereqs table" << endl;
	}
}


void AdvisingDBAbstraction::createPlansTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS 'Plans' ( `PlanID` INTEGER, `PlanName` TEXT, `Semester` TEXT, `Year` INTEGER, `AdvisorID` INTEGER, `StudentID` INTEGER, FOREIGN KEY(`StudentID`) REFERENCES `Students`(`StudentID`), FOREIGN KEY(`AdvisorID`) REFERENCES `Advisors`(`AdvisorID`), PRIMARY KEY(`PlanID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Plans table" << endl;
	}
}


void AdvisingDBAbstraction::createCoursesInPlanTable()
{
	string sql = "CREATE TABLE IF NOT EXISTS 'CoursesInPlan' ( `CourseID` INTEGER, `PlanID` INTEGER, FOREIGN KEY(`CourseID`) REFERENCES `Courses`(`CourseID`), PRIMARY KEY(`PlanID`,`CourseID`), FOREIGN KEY(`PlanID`) REFERENCES `Plans`(`PlanID`) )";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error creating Courses In Plan table" << endl;
	}
}

void AdvisingDBAbstraction::createPlanIndex()
{
	string sql = "CREATE INDEX IF NOT EXISTS PlansSemester_Year ON Plans(Semester, Year)";

	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error createing Plan Index" << endl;
	}
}


void AdvisingDBAbstraction::insertAdvisor(string firstName, string lastName)
{
	string sql = "INSERT INTO Advisors (firstName, lastName) VALUES (?, ?);";
	sqlite3_stmt* myStatement;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);
	if (statusOfPrep == SQLITE_OK)
	{
		sqlite3_bind_text(myStatement, 1, firstName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(myStatement, 2, lastName.c_str(), -1, SQLITE_STATIC);
		if (!executeQueryNoResultsBack(myStatement))
		{
			cout << "Error inserting into Advisors" << endl;
		}
	}
}

void AdvisingDBAbstraction::insertStudent(string firstName, string lastName, int AdvisorID)
{
	//insert student
	string sql = "INSERT INTO Students (firstName, lastName, AdvisorID) VALUES (?, ?, ?);";

	//statement pointer
	sqlite3_stmt* myStatement;

	//prepare statement
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

	//check for creation
	if (statusOfPrep == SQLITE_OK)
	{
		//add parameters
		sqlite3_bind_text(myStatement, 1, firstName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(myStatement, 2, lastName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(myStatement, 3, AdvisorID);

		if (!executeQueryNoResultsBack(myStatement))
		{
			cout << "Error inserting into Advisors" << endl;
		}
	}
}

void AdvisingDBAbstraction::insertCourse(int Number, string CourseName, int NumCredits, bool Required, vector<int> PreReqs)
{
		//query for inserting course
		string sql = "INSERT INTO Courses (Number, CourseName,NumCredits, Required) VALUES (?, ?, ?, ?);";

		//statement pointer
		sqlite3_stmt* myStatement;

		//prepare statement
		int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

		//check for creation
		if (statusOfPrep == SQLITE_OK)
		{
			//add parameters
			sqlite3_bind_int(myStatement, 1, Number);
			sqlite3_bind_text(myStatement, 2, CourseName.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(myStatement, 3, NumCredits);
			sqlite3_bind_int(myStatement, 4, Required);

			if (!executeQueryNoResultsBack(myStatement))
			{
				cout << "Error inserting into Courses" << endl;
			}
		}
		int courseID = sqlite3_last_insert_rowid(db);

		//insert all prereqs
		if (PreReqs.size() != 0)
		{
			for (auto& PreReqID : PreReqs)
			{
				sql = "INSERT INTO CoursePrereqs (CourseID, PrereqID) VALUES (?, ?);";
				statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

				if (statusOfPrep == SQLITE_OK)
				{
					sqlite3_bind_int(myStatement, 1, courseID);
					sqlite3_bind_int(myStatement, 2, PreReqID);

					if (!executeQueryNoResultsBack(myStatement))
					{
						cout << "Error inserting Prerequisite for Course" << courseID << endl;
					}
				}
			}
		}
}

void AdvisingDBAbstraction::insertPlan(string planName, int year, string semester, int studentID, int advisorID, vector<int> coursesInPlan)
{
	//insert Plan
	string sql = "INSERT INTO Plans (PlanName, Semester, Year, AdvisorID, StudentID) VALUES (?, ?, ?, ?, ?);";

	//statement pointer
	sqlite3_stmt* myStatement;

	//prepare statement
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

	//check for creation
	if (statusOfPrep == SQLITE_OK)
	{
		//add parameters
		sqlite3_bind_text(myStatement, 1, planName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(myStatement, 2, semester.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(myStatement, 3, year);
		sqlite3_bind_int(myStatement, 4, advisorID);
		sqlite3_bind_int(myStatement, 5, studentID);

		if (!executeQueryNoResultsBack(myStatement))
		{
			cout << "Error inserting into Plans" << endl;
		}
	}
	int planID = sqlite3_last_insert_rowid(db);

	//add courses in plan
	if (coursesInPlan.size() != 0)
	{
		for (auto& courseID : coursesInPlan)
		{
			sql = "INSERT INTO CoursesInPlan (CourseID, PlanID) VALUES (?, ?);";
			statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

			//Insert all courses into coursesInPlan
			if (statusOfPrep == SQLITE_OK)
			{
				sqlite3_bind_int(myStatement, 1, courseID);
				sqlite3_bind_int(myStatement, 2, planID);

				if (!executeQueryNoResultsBack(myStatement))
				{
					cout << "Error inserting courses In Plan" << courseID << endl;
				}
			}
		}
	}
}

void AdvisingDBAbstraction::insertCourseTaken(string semester, int year, int CourseID, int studentID)
{
	//insert CourseTaken
	string sql = "INSERT INTO CoursesTaken (Semester, Year, CourseID, StudentID) VALUES (?, ?, ?, ?);";
	
	//statement pointer
	sqlite3_stmt* myStatement;

	//prepare statement
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

	//check for creation
	if (statusOfPrep == SQLITE_OK)
	{
		//add parameters
		sqlite3_bind_text(myStatement, 1, semester.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(myStatement, 2, year);
		sqlite3_bind_int(myStatement, 3, CourseID);
		sqlite3_bind_int(myStatement, 4, studentID);

		if (!executeQueryNoResultsBack(myStatement))
		{
			cout << "Error inserting into CoursesTaken" << endl;
		}
	}
}

void AdvisingDBAbstraction::populateDatabase()
{
	if (populatedDatabase == false)
	{
		insertAdvisor("Mark", "Mahoney");
		insertAdvisor("Perry", "Kivolowitz");
		insertAdvisor("Torchie", "TheMascot");

		insertStudent("Quade", "Kayle", 1);
		insertStudent("Charles", "Gallagher", 1);
		insertStudent("Juan", "Sanchez", 2);
		insertStudent("Mike", "York", 2);
		insertStudent("Logan", "Engberg", 3);

		insertCourse(1810, "Principles of CS 1", 4, 1);
		insertCourse(1820, "Principles of CS 2", 4, 1, { 1 });
		insertCourse(2560, "Data Structures And Algorithms", 4, 1, { 1 , 2 });
		insertCourse(3510, "CompOrg", 4, 1, { 1, 2 });
		insertCourse(4350, "Software Design and Development", 4, 1, { 1, 2, 3, 4});
		insertCourse(4730, "Operating Systems", 4, 1, { 1, 2, 3, 4 });


		insertCourse(2810, "Database Design (super fun)", 4, 0, { 1, 2});
		insertCourse(2910, "Object Oriented Programming", 4, 0, { 1, 2, 3});
		insertCourse(3530, "Artificial Intelligence", 4, 0, { 1, 2, 3, 8});
		insertCourse(4650, "Computer Architecture", 4, 0, { 1, 2, 3, 8});
		insertCourse(3750, "Web Development", 4, 0, { 1, 2, 7, 8 });

		insertCourseTaken("Spring", 2017, 8, 1);
		insertCourseTaken("Spring", 2016, 1, 1);
		insertCourseTaken("Fall", 2016, 2, 1);
		insertCourseTaken("Fall", 2017, 3, 1);
		insertCourseTaken("Spring", 2017, 7, 1);
		insertCourseTaken("Spring", 2017, 8, 2);
		insertCourseTaken("Spring", 2016, 1, 2);
		insertCourseTaken("Fall", 2016, 2, 2);
		insertCourseTaken("Fall", 2017, 3, 2);
		insertCourseTaken("Spring", 2017, 7, 2);
		insertCourseTaken("Spring", 2017, 8, 3);
		insertCourseTaken("Spring", 2016, 1, 3);
		insertCourseTaken("Fall", 2016, 2, 3);
		insertCourseTaken("Fall", 2017, 3, 3);
		insertCourseTaken("Spring", 2017, 7, 3);
		insertCourseTaken("Fall", 2017, 1, 4);
		insertCourseTaken("Spring", 2018, 2, 4);
		insertCourseTaken("Spring", 2016, 1, 5);
		insertCourseTaken("Fall", 2016, 2, 5);
		insertCourseTaken("Spring", 2017, 3, 5);
		insertCourseTaken("Spring", 2017, 4, 5);

		insertPlan("Spring2019Quade", 2019, "Spring", 1, 1, { 11, 10 });
		insertPlan("Spring2019Logan", 2019, "Spring", 5, 3, { 5, 6 });
		insertPlan("Spring2019Mike", 2019, "Spring", 4, 2, { 3, 4 });


		populatedDatabase = true;

	}
}

void AdvisingDBAbstraction::printAdvisors()
{
	sqlite3_stmt* statement;
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, "SELECT * FROM ADVISORS", -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			cout << "NAME: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_text(statement, 2) <<  endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Advisors" << endl;
	}
}

void AdvisingDBAbstraction::printCourses()
{
	sqlite3_stmt* statement;
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, "SELECT * FROM Courses", -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			cout << "Number: " << sqlite3_column_int(statement, 1) << "   ";
			cout << "Course Name: " << sqlite3_column_text(statement, 2) << "   ";
			cout << "Credits: " << sqlite3_column_int(statement, 3) << "   ";
			cout << "Required: " << sqlite3_column_int(statement, 4) << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Advisors" << endl;
	}
}

void AdvisingDBAbstraction::addEvent(string evnt)
{
	history.push_back(evnt);
}

void AdvisingDBAbstraction::printHistory()
{
	system("cls");
	cout << "--- SESSION HISTORY ---" << endl;
	for (auto& evnt : history)
	{
		cout << evnt << endl;
	}
	cout << endl;
}

vector<int> AdvisingDBAbstraction::printAndGetStudents()
{
	sqlite3_stmt* statement;

	vector<int> retval = vector<int>();
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, "SELECT * FROM Students", -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			retval.push_back(sqlite3_column_int(statement, 0));
			cout << "Name: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_text(statement, 2) << "   " << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Students" << endl;
	}

	return retval;
}

vector<int> AdvisingDBAbstraction::printAndGetAdvisors()
{
	sqlite3_stmt* statement;

	vector<int> retval = vector<int>();
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, "SELECT * FROM Advisors", -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			retval.push_back(sqlite3_column_int(statement, 0));
			cout << "Name: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_text(statement, 2) << "   " << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Students" << endl;
	}

	return retval;
}

vector<int> AdvisingDBAbstraction::printCoursesTaken(int StudentID)
{
	sqlite3_stmt* statement;

	vector<int> retval = vector<int>();
	string sql = "SELECT Courses.CourseID, Courses.CourseName, Courses.Number, CoursesTaken.Semester, CoursesTaken.Year FROM Courses, CoursesTaken Where Courses.CourseID = CoursesTaken.CourseID AND CoursesTaken.StudentID = " + to_string(StudentID);
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str() , -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			retval.push_back(sqlite3_column_int(statement, 0));
			cout << "Course Name & Number: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_int(statement, 2) << "   ";
			cout << "Semester And Year Taken: " << sqlite3_column_text(statement, 3) << " " << sqlite3_column_int(statement, 4) << "   " << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing CoursesTaken" << endl;
	}
	cout << endl;
	return retval;
}

vector<int> AdvisingDBAbstraction::printAndGetCoursesAvaliable(int studentID)
{
	sqlite3_stmt* statement;

	vector<int> retval = vector<int>();
	string sql = "SELECT Courses.CourseID, Courses.CourseName, Courses.Number, Courses.NumCredits, Courses.Required From Courses, CoursePrereqs Where Courses.CourseID = CoursePrereqs.CourseID and CoursePrereqs.PrereqId In( SELECT Courses.CourseID FROM Courses, CoursesTaken Where Courses.CourseID = CoursesTaken.CourseID  AND CoursesTaken.StudentID = " + to_string(studentID) + ") AND Courses.CourseID  Not In (SELECT Courses.CourseID From Courses, CoursePrereqs Where Courses.CourseID = CoursePrereqs.CourseID and CoursePrereqs.PrereqId Not In(SELECT Courses.CourseID FROM Courses, CoursesTaken Where Courses.CourseID = CoursesTaken.CourseID  AND CoursesTaken.StudentID = " + to_string(studentID) + ")) AND Courses.CourseId Not in (SELECT Courses.CourseID FROM Courses, CoursesTaken Where Courses.CourseID = CoursesTaken.CourseID  AND CoursesTaken.StudentID = " + to_string(studentID) + ") Or (Courses.CourseID Not IN (Select CoursePrereqs.CourseID From CoursePrereqs) AND Courses.CourseId Not in  (SELECT Courses.CourseID FROM Courses, CoursesTaken  Where Courses.CourseID = CoursesTaken.CourseID  AND CoursesTaken.StudentID = " + to_string(studentID) + ")) group by courses.courseID";
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			retval.push_back(sqlite3_column_int(statement, 0));
			cout << "Course Name & Number: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_int(statement, 2) << "   ";
			cout << "Credits: " << sqlite3_column_text(statement, 3);
			if (sqlite3_column_int(statement, 4) == 1)
			{
				cout << " *REQUIRED";
			}
			cout << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Avaliable Courses" << endl;
	}
	cout << endl;
	return retval;
}

vector<int> AdvisingDBAbstraction::printAndGetCoursesUnavaliable(int studentID)
{
	sqlite3_stmt* statement;

	vector<int> retval = vector<int>();
	string sql = "SELECT Courses.CourseID, Courses.CourseName, Courses.Number, Courses.NumCredits From Courses, CoursePrereqs Where Courses.CourseID = CoursePrereqs.CourseID and CoursePrereqs.PrereqId Not In(SELECT Courses.CourseID FROM Courses, CoursesTaken Where Courses.CourseID = CoursesTaken.CourseID  AND CoursesTaken.StudentID = " + to_string(studentID) + ")group by courses.courseID";
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "ID: " << sqlite3_column_int(statement, 0) << "   ";
			retval.push_back(sqlite3_column_int(statement, 0));
			cout << "Course Name & Number: " << sqlite3_column_text(statement, 1) << " " << sqlite3_column_int(statement, 2) << "   ";
			cout << "Credits: " << sqlite3_column_text(statement, 3) << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Avaliable Courses" << endl;
	}
	cout << endl;
	return retval;
}

void AdvisingDBAbstraction::printRequiredCoursesSum(int studentID)
{
	sqlite3_stmt* statement;
	string sql = "select sum(Courses.NumCredits) from Courses, CoursesTaken where Courses.CourseID = CoursesTaken.CourseID and Courses.Required = 1 And CoursesTaken.StudentID = " + to_string(studentID);
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "Required Course Credits: " << sqlite3_column_int(statement, 0) << "   " << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Required Course Credit Sum" << endl;
	}
	cout << endl;
}

void AdvisingDBAbstraction::printNonRequiredCoursesSum(int studentID)
{
	sqlite3_stmt* statement;
	string sql = "select sum(Courses.NumCredits) from Courses, CoursesTaken where Courses.CourseID = CoursesTaken.CourseID and Courses.Required = 0 And CoursesTaken.StudentID = " + to_string(studentID);
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "Non-Required Course Credits: " << sqlite3_column_int(statement, 0) << "   " << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Non-Required Course Credit Sum" << endl;
	}
	cout << endl;
}

AdvisingDBAbstraction::Plan AdvisingDBAbstraction::getPlan(int PlanID)
{
	Plan retval;
	
	sqlite3_stmt* statement;
	string sql = "select * from Plans Where Plans.PlanId = " + to_string(PlanID);
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			retval.PlanID = sqlite3_column_int(statement, 0);
			retval.PlanName = (char*)sqlite3_column_text(statement, 1);
			retval.Semester = (char*)sqlite3_column_text(statement, 2);
			retval.year = sqlite3_column_int(statement, 3);
			retval.advisorID = sqlite3_column_int(statement, 4);
			retval.studentID = sqlite3_column_int(statement, 5);
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error getting Plan" << endl;
	}

	sql = "select * from Courses,CoursesInPlan where CoursesInPlan.CourseID = Courses.CourseID AND CoursesInPlan.PlanID = " + to_string(PlanID);
	statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			Course course;
			course.courseID = sqlite3_column_int(statement, 0);
			course.Number = sqlite3_column_int(statement, 1);
			course.CourseName = (char*)sqlite3_column_text(statement, 2);
			course.NumCredits= sqlite3_column_int(statement, 3);
			course.Required = sqlite3_column_int(statement, 4);
			retval.courses.push_back(course);
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error getting Plan" << endl;
	}

	cout << endl;
	return retval;

}

void AdvisingDBAbstraction::insertCoursesInPlan(int PlanID, vector<int> courses)
{
	//insert course in plan
	string sql;

	//statement pointer
	sqlite3_stmt* myStatement;

	//prepare statement
	int statusOfPrep;

	if (courses.size() != 0)
	{
		for (auto& courseID : courses)
		{
			sql = "INSERT INTO CoursesInPlan (CourseID, PlanID) VALUES (?, ?);";
			//prepare each course
			statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &myStatement, NULL);

			if (statusOfPrep == SQLITE_OK)
			{
				sqlite3_bind_int(myStatement, 1, courseID);
				sqlite3_bind_int(myStatement, 2, PlanID);

				if (!executeQueryNoResultsBack(myStatement))
				{
					cout << "Error inserting courses In Plan" << courseID << endl;
				}
			}
		}
	}
}

void AdvisingDBAbstraction::deletePlan(int PlanID)
{
	string sql = "Delete From CoursesInPlan Where CoursesInPlan.PlanID = " + to_string(PlanID);
	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error deleteing Courses In Plan" << PlanID << endl;
	}
	sql = "Delete From Plans Where Plans.PlanID = " + to_string(PlanID);
	if (!executeQueryNoResultsBack(sql))
	{
		cout << "Error deleteing Plan" << PlanID << endl;
	}
}

void AdvisingDBAbstraction::convertPlanToCoursesTaken(Plan plan)
{
	//begin a transacation
	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	//insert courses taken From Plan
	for (auto& course : plan.courses)
	{
		insertCourseTaken(plan.Semester, plan.year, course.courseID, plan.studentID);
	}

	deletePlan(plan.PlanID);
	//end the transaction
	sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
}

vector<int> AdvisingDBAbstraction::getPlans(int advisorID)
{
	sqlite3_stmt* statement;
	vector<int> plans;
	string planName;
	string sql = "select Plans.PlanID, Plans.PlanName from Plans Where Plans.AdvisorID = " + to_string(advisorID);
	int statusOfStep;
	int statusOfPrep = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	statusOfStep = sqlite3_step(statement);
	if (statusOfPrep == SQLITE_OK)
	{
		while (statusOfStep == SQLITE_ROW)
		{
			cout << "Plan ID: " << sqlite3_column_int(statement, 0) << "   ";
			plans.push_back(sqlite3_column_int(statement, 0));
			cout << "Plan Name: " << sqlite3_column_text(statement, 1) << endl;
			statusOfStep = sqlite3_step(statement);
		}
	}
	else
	{
		cout << "Error Printing Non-Required Course Credit Sum" << endl;
	}
	cout << endl;
	return plans;
}

void AdvisingDBAbstraction::makePlan()
{
	int studentID = 0;
	int advisorID = 0;
	int courseID = 0;
	string planSemester;
	int planYear;
	string planName;
	vector<int> coursesInPlan;

	cout << "Please enter a name for the plan :"; cin >> planName;
	cout << "Please enter an Semester for the plan: "; cin >> planSemester;
	cout << "Please enter an Year for the plan: "; cin >> planYear;

	vector<int> advisors = printAndGetAdvisors();

	cout << endl << "Please enter an Advisor ID for the plan: "; cin >> advisorID;

	if (!(find(advisors.begin(), advisors.end(), advisorID) != advisors.end()))
	{
		cout << endl << "Advisor ID was not valid. Cancelling Plan." << endl;
		system("pause");
		return;
	}

	vector<int> students = printAndGetStudents();

	cout << endl << endl << "Please enter a Student ID to make a plan for: "; cin >> studentID;

	if (find(students.begin(), students.end(), studentID) != students.end())
	{
		cout << "--- Courses Taken by Student ID: " << studentID << " ---" << endl;
		vector<int> coursesTaken = printCoursesTaken(studentID);

		cout << "--- Courses Avaliable for Student ID: " << studentID << " ---" << endl;
		vector<int> coursesAvaliable = printAndGetCoursesAvaliable(studentID);
		
		cout << "--- Courses Unavaliable for Student ID: " << studentID << " ---" << endl;
		vector<int> coursesUnavaliable = printAndGetCoursesUnavaliable(studentID);

		cout << "--- Credits in Required Courses ---" << endl;
		printRequiredCoursesSum(studentID);

		cout << "--- Credits in Non-required Courses ---" << endl;
		printNonRequiredCoursesSum(studentID);

		while (courseID != -1)
		{
			cout << endl;
			cout << "Please choose a course to add to the plan or -1 to finish plan: "; cin >> courseID;
			if (courseID != -1)
			{
				if (find(coursesAvaliable.begin(), coursesAvaliable.end(), courseID) != coursesAvaliable.end() && !(find(coursesInPlan.begin(), coursesInPlan.end(), courseID) != coursesInPlan.end()))
				{
					coursesInPlan.push_back(courseID);
					cout << "Course " << courseID << " has been added to the student's plan." << endl << endl;
				}
				else
				{
					cout << "This course could not be added. " << endl << endl;
				}
			}
		}

		insertPlan(planName, planYear, planSemester, studentID, advisorID, coursesInPlan);
	}
	else {
		cout << endl << "Student ID was not valid." << endl;
		system("pause");
	}

}

void AdvisingDBAbstraction::PlanManager()
{
	vector<int> advisors = printAndGetAdvisors();
	int advisorID;
	int planID = 0;
	int courseID = 0;
	Plan currentPlan;
	string evnt;

	cout << endl << "Plans for Advisor ID: "; cin >> advisorID;

	if (!(find(advisors.begin(), advisors.end(), advisorID) != advisors.end()))
	{
		cout << endl << "Advisor ID was not valid. Exiting Plan Manager." << endl;
		system("pause");
		return;
	}
	vector<int> plans = getPlans(advisorID);
	int option2 = 0;

	while (planID != -1) {
		option2 = 0;
		cout << "Choose plan or type -1 to exit: "; cin >> planID;
		if (find(plans.begin(), plans.end(), planID) != plans.end())
		{
			currentPlan = getPlan(planID);
			while (option2 != -1)
			{
				courseID = 0;
				cout << "--- Options ---" << endl;
				cout << "1: Add Courses to Plan" << endl;
				cout << "2: View Plan" << endl;
				cout << "3: Delete Plan" << endl;
				cout << "4: Convert Plan to Courses Taken" << endl;
				cout << "-1: Quit" << endl;

				cout << endl << "Option: "; cin >> option2;
				switch (option2) {
					case 1: {
						vector<int> coursesToAdd;
						cout << "--- Courses Taken by Student ID: " << currentPlan.studentID << " ---" << endl;
						vector<int> coursesTaken = printCoursesTaken(currentPlan.studentID);

						cout << "--- Courses Avaliable for Student ID: " << currentPlan.studentID << " ---" << endl;
						vector<int> coursesAvaliable = printAndGetCoursesAvaliable(currentPlan.studentID);

						cout << "--- Courses Unavaliable for Student ID: " << currentPlan.studentID << " ---" << endl;
						vector<int> coursesUnavaliable = printAndGetCoursesUnavaliable(currentPlan.studentID);

						cout << "--- Courses Already In Plan for Student ID: " << currentPlan.studentID << " ---" << endl;
						for (auto& course : currentPlan.courses)
						{
							cout << "Course ID: " << course.courseID << "   CourseName and Number: " << course.CourseName << " " << course.Number << "   Credits: " << course.NumCredits;
							if (course.Required == 1)
							{
								cout << " *REQUIRED";
							}
							cout << endl;
						}

						cout << endl << "--- Credits in Required Courses ---" << endl;
						printRequiredCoursesSum(currentPlan.studentID);

						cout << "--- Credits in Non-required Courses ---" << endl;
						printNonRequiredCoursesSum(currentPlan.studentID);

						while (courseID != -1)
						{
							cout << endl;
							cout << "Please choose a course to add to the plan or -1 to finish plan: "; cin >> courseID;
							if (courseID != -1)
							{
								if (find(coursesAvaliable.begin(), coursesAvaliable.end(), courseID) != coursesAvaliable.end() && !(find(coursesToAdd.begin(), coursesToAdd.end(), courseID) != coursesToAdd.end()) && !(find(currentPlan.courses.begin(), currentPlan.courses.end(), courseID) != currentPlan.courses.end()) )
								{
									coursesToAdd.push_back(courseID);
									cout << "Course " << courseID << " has been added to the student's plan." << endl << endl;
								}
								else
								{
									cout << "This course could not be added. " << endl << endl;
								}
							}
						}//Add the courses in coursesToAdd

						insertCoursesInPlan(currentPlan.PlanID, coursesToAdd);
						cout << "Plan " << currentPlan.PlanID << " Updated." << endl << endl;
						currentPlan = getPlan(currentPlan.PlanID);
						evnt = "Updated Plan " + to_string(currentPlan.PlanID) + " with More Courses";
						addEvent(evnt);
						cout << "Successfully " << evnt << endl << endl;
						break;
					}
					case 2: {
						cout << endl << "Plan Id: " << currentPlan.PlanID << "   Plan Name: " << currentPlan.PlanName << "   Semester: " << currentPlan.Semester << "   Year: " << currentPlan.year << "   StudentID: " << currentPlan.studentID << "   AdvisorID: " << currentPlan.advisorID << endl;
						cout << "--- COURSES IN PLAN ---" << endl;
						for (auto& course : currentPlan.courses)
						{
							cout << "Course ID: " << course.courseID << "   CourseName and Number: " << course.CourseName << " " << course.Number << "   Credits: " << course.NumCredits;
							if (course.Required == 1)
							{
								cout << " *REQUIRED";
							}
							cout << endl;
						}
						cout << endl;
						evnt = "Viewed Plan " + to_string(currentPlan.PlanID);
						addEvent(evnt);
						cout << "Successfully " << evnt << endl << endl;
						break;
					}
					case 3: {
						char option = 'o';
						cout << endl << "ARE YOU SURE YOU WANT TO DELETE THIS PLAN? (Y/N): "; cin >> option;
						if (option == 'Y')
						{
							deletePlan(currentPlan.PlanID);
							option2 = -1;
							planID = -1;
							evnt = "Deleted Plan " + to_string(currentPlan.PlanID);
							addEvent(evnt);
							cout << "Successfully " << evnt << endl << endl;
						}
						else {
							break;
						}
						break;
					}
					case 4: {
						char option = 'o';
						cout << endl << "ARE YOU SURE YOU WANT TO CONVERT THIS PLAN? (Y/N): "; cin >> option;
						if (option == 'Y')
						{
							convertPlanToCoursesTaken(currentPlan);
							option2 = -1;
							planID = -1;
							evnt = "Converted Plan " + to_string(currentPlan.PlanID) + " to Courses Taken";
							addEvent(evnt);
							cout << "Successfully " << evnt << endl << endl;
						}
						else {
							break;
						}
					}
				}
			}
		}
		else
		{
			cout << "Please choose a correct PlanID " << endl;
		}
	}
}


