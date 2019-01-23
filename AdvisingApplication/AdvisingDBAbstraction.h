#pragma once

#include "DBAbstraction.h"
#include<vector>
class AdvisingDBAbstraction : DBAbstraction
{
	public:
		AdvisingDBAbstraction(string pathToFile);
		~AdvisingDBAbstraction();

		void insertAdvisor(string firstName, string lastName);
		void insertStudent(string firstName, string lastName, int AdvisorID);
		void insertCourse(int Number, string CourseName, int NumCredits, bool Required, vector<int> PreReqs = vector<int>());
		void insertPlan(string planName, int year, string semester, int studentID, int advisorID, vector<int> coursesInPlan);
		void insertCourseTaken(string semester, int year, int CourseID, int studentID);

		void populateDatabase();

		void printAdvisors();
		void printCourses();

		void makePlan();
		void PlanManager();

		void addEvent(string evnt);
		void printHistory();

	private:
		void createAdvisorsTable();
		void createStudentsTable();
		void createCoursesTable();
		void createCoursesTakenTable();
		void createCoursePrereqsTable();
		void createPlansTable();
		void createCoursesInPlanTable();
		void createPlanIndex();



		vector<int> printAndGetStudents();
		vector<int> printAndGetAdvisors();
		vector<int> printCoursesTaken(int StudentID);
		vector<int> printAndGetCoursesAvaliable(int studentID);
		vector<int> printAndGetCoursesUnavaliable(int studentID);
		void printRequiredCoursesSum(int studentID);
		void printNonRequiredCoursesSum(int studentID);
		
		vector<int> getPlans(int advisorID);

		//Data Structures
		struct Course {
			int courseID;
			int Number;
			string CourseName;
			int NumCredits;
			int Required;

			bool operator ==(int num)
			{
				return courseID == num;
			}
		};
		struct Plan {
			int PlanID;
			string PlanName;
			string Semester;
			int year;
			int studentID;
			int advisorID;
			vector<Course> courses;
		};

		Plan getPlan(int PlanID);
		void insertCoursesInPlan(int PlanID, vector<int> courses);
		void deletePlan(int PlanID);
		void convertPlanToCoursesTaken(Plan plan);

		bool populatedDatabase = false;
		vector<string> history;
};

