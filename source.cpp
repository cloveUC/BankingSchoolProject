#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<vector>
#include<ctime>
#include"Header.h"

using namespace std;
vector<Customer> cus;
vector<Admin> ad;
vector<Customer>readcustextfile(string);
vector<Admin>readadtextfile(string);

double Customer::withdrawal(double out) {
	double ncb = cb - out;
	setCB(ncb);
	if (ncb >= 0) {
		std::cout << "Transaction successfully. Your current balance: $" << ncb << endl;
	}
	else {
		std::cout << "You have a negative balance and you will have to pay interest" << endl;
		message.push_back("You have a debt of " + to_string(ncb));
	}
	message.push_back(to_string(out) + 'W');
	transactions.push_back(getCB());
	return ncb;
}

double Customer::deposit(double d) {
	double nb = cb + d;
	setCB(nb);
	message.push_back(to_string(d) + 'D');
	transactions.push_back(getCB());
	std::cout << "You have deposited $" << d <<". Your current balance is $" << nb;
	return nb;
}

Customer* findCID(long& cid, vector<Customer>& c) {
	for (int i = 0; i < c.size(); i++) {
		if (c[i].getCID() == cid) {
			return &c[i];
		}
	}
	return nullptr;
}

bool checkcidexistence(long to_cid) {
	bool check = false;
	vector<Customer> c = readcustextfile("users_info.txt");
	Customer* to_cus = findCID(to_cid, c);
	if (to_cus) {
		check = true;
	}
	return check;
}

double Customer::transfer(double m, long to_cid) {
	vector<Customer> cust = readcustextfile("users_info.txt");
	Customer* to_cus = findCID(to_cid, cust);
	bool check = checkcidexistence(to_cid);
	while (check == false) {
		std::cout << "CID not found, please re-enter CID: "; cin >> to_cid;
	}
	if (cb >= m) {
		double ncb = cb - m;
		setCB(ncb);
		std::cout << "Transaction successfully" << endl;
		message.push_back(to_string(m) + "T");
		transactions.push_back(getCB());
		double to_cus_ncb = to_cus->cb + m;
		to_cus->setCB(to_cus_ncb);
		to_cus->message.push_back("You receive " + to_string(m));
		to_cus->transactions.push_back(to_cus->getCB());
	}
	else {
		std::cout << "You don't have enough money, transaction unsuccessfully!";
		message.push_back("Transaction failed");
	}
	return cb;
}

void Customer::showInboxMessages() {
	for (int i = 0; i < message.size(); i++) {
		std::cout << message[i] << endl;
	}
	for (int k = 0; k < warning.size(); k++) {
		std::cout << warning[k] << endl;
	}
}

void Customer::lastNTransactions() {
	for (int i = 0; i < transactions.size(); i++) {
		std::cout << transactions[i] << endl;
	}
}

int rannum() {
	srand(time(nullptr));
	return rand() % 10000;
}

//randomly create the usernames for new customer
string username(string fn, string ln) {
	string usn;
	int ran = rannum();
	usn = ln + fn + to_string(ran);
	vector<Customer>c = readcustextfile("users_info.txt");
	vector<Admin>a = readadtextfile("List_of_Admin.txt");
	for (int i = 0; i < c.size(); i++) {
		for (int j = 0; j < a.size(); j++) {
			while (usn == c[i].getUSN() || usn == a[j].getUSN()) {
				usn = ln + fn + to_string(ran);
			}
		}
	}
	return usn;
}

int rannumforid() {
	srand(time(nullptr));
	return rand() % 10000;
}

string encrypt(string str) {
	int l = str.length();
	char t;
	string nstr;
	for (int i = 0; i < l; i++) {
		t = str[i] + i;
		nstr += t;
	}
	return nstr;
}

string decrypt(string str) {
	int l = str.length();
	char t;
	string nstr;
	for (int i = 0; i < l; i++) {
		t = str[i] - i;
		nstr += t;
	}
	return nstr;
}

void generate_report(Customer c) {
	ofstream file("account_balance_report.txt", ios::app);
	vector<Customer>cus = readcustextfile("users_info.txt");
	if (file.is_open()) {
		for (int i = 0; i < cus.size(); i++) {
			
			file << to_string(cus[i].getCID()) << endl;
			file << to_string(cus[i].lasttrans()) << endl;
			file << "+++++++++++++++++++++" << endl;
		}
	}
	else {
		cout << "Error opening the file";
	}
}

vector<Customer> read_report(string filename) {
	ifstream file;
	file.open(filename, ios::in);
	vector<Customer>cus;
	if (!file.is_open()) {
		cout << "Failed opening the file";
	}
	else {
		Customer c;
		while (!file.eof()) {
			string cid, cb, ignore;
			getline(file, cid);
			long CID = stol(cid);
			c.setCID(CID);
			getline(file, cb);
			double CB = stod(cb);
			c.setCB(CB);
			getline(file, ignore);
			cus.push_back(c);
		}
		file.close();
	}
	return cus;
}

double Admin::payInterest(vector<Customer>c) {
	double totalInterest = 0.0;
	for (int i = 0; i < c.size(); i++) {
		if (c[i].getCB() >= 0) {
			double interest = c[i].getCB() * 0.03;
			double nb = c[i].getCB() + interest;
			c[i].setCB(nb);
			c[i].push_backcb(nb);
			totalInterest += interest;
		}
	}
	std::cout << "Done paying interest\n";
	return totalInterest;
}

void Admin::sendwarning(vector<Customer>c) {
	for (int i = 0; i < c.size(); i++) {
		if (c[i].getCB() < 0) {
			string message = "Your current ballance is negative. Please pay the money or your account will be blocked!";
			c[i].push_backmessage(message);
		}
	}
	std::cout << "Warning sent! Return in 30 days." << endl;
}

void Admin::blockedaccounts(vector<Customer>c) {
	ofstream report("negativeaccounts.txt", ios::app);
	for (int i = 0; i < c.size(); i++) {
		int n = c[i].countwarnings();
		if (n >= 3) {
			c[i].setStat("Inactive");
			cout << "Deactivate neagative accounts successfully" << endl;
		}
	}
}

void generatesummaryreport(Customer customer) {
	ofstream myfile("users_info.txt", ios::out | ios::app);
	if (myfile.is_open()) {
		string cid = to_string(customer.getCID());
		myfile << cid << endl;
		myfile << encrypt(customer.getUSN()) << endl;
		myfile << encrypt(customer.getPass()) << endl;
		myfile << encrypt(customer.getSSN()) << endl;
		myfile << encrypt(customer.getFN()) << endl;
		myfile << encrypt(customer.getLN()) << endl;
		myfile << encrypt(customer.getORG()) << endl;
		myfile << encrypt(customer.getDOB()) << endl;
		myfile << encrypt(customer.getDOJ()) << endl;
		myfile << "-----------------------------------" << endl;
		myfile.close();
		std::cout << "Writing to file completed!";
	}
	else {
		std::cout << "File can't be opened!";
	}
}

void adminlist(Admin a) {
	ofstream file("List_of_Admin.txt", ios::out | ios::app);
	if (file.is_open()) {
		string aid = to_string(a.getAID());
		file << aid << endl;
		file << encrypt(a.getUSN()) << endl;
		file << encrypt(a.getPass()) << endl;
		file << encrypt(a.getSSN()) << endl;
		file << encrypt(a.getFN()) << endl;
		file << encrypt(a.getLN()) << endl;
		file << encrypt(a.getORG()) << endl;
		file << encrypt(a.getDOB()) << endl;
		file << encrypt(a.getDOJ()) << endl;
		file << "-------------------------------------" << endl;
		file.close();
		std::cout << "Completed";
	}
	else {
		std::cout << "Can't open file";
	}
}

vector<Customer> readcustextfile(string file_name) {
	vector<Customer>customer;
	ifstream file;
	file.open(file_name, ios::in);
	if (!file.is_open()) {
		std::cout << "Failed to open the file" << endl;
	}
	else {
		Customer c;
		while (!file.eof()) {
			string cid, cb, usn, pass, ssn, fn, ln, org, dob, doj;
			//Read line 1
			getline(file, cid);
			long n = stol(cid);
			c.setCID(n);
			//Read line 2
			getline(file, usn);
			string USN = decrypt(usn);
			c.setUSN(USN);
			//Read line 3
			getline(file, pass);
			string PASS = decrypt(pass);
			c.setPass(PASS);
			//Read line 4
			getline(file, ssn);
			string SSN = decrypt(ssn);
			c.setSSN(SSN);
			//Read line 5
			getline(file, fn);
			string FN = decrypt(fn);
			c.setFN(FN);
			//Read line 6
			getline(file, ln);
			string LN = decrypt(ln);
			c.setLN(LN);
			//Read line 7
			getline(file, org);
			string ORG = decrypt(org);
			c.setORG(ORG);
			//Read line 8
			getline(file, dob);
			string DOB = decrypt(dob);
			c.setDOB(DOB);
			//Read line 9
			getline(file, doj);
			string DOJ = decrypt(doj);
			c.setDOJ(DOJ);
			//ignore line 10;
			string ignore;
			getline(file, ignore);
			customer.push_back(c);
		}
		file.close();
	}
	return customer;
}

vector<Admin> readadtextfile(string file_name) {
	vector<Admin>admin;
	ifstream file;
	file.open(file_name, ios::in);
	string input;
	if (!file.is_open()) {
		std::cout << "Failed to open the file" << endl;
	}
	else {
		Admin a;
		while (!file.eof()) {
			string aid, cb, usn, pass, ssn, fn, ln, org, dob, doj;
			//Read line 1
			getline(file, aid);
			long n = stol(aid);
			a.setAID(n);
			//Read line 2
			getline(file, usn);
			string USN = decrypt(usn);
			a.setUSN(USN);
			//Read line 3
			getline(file, pass);
			string PASS = decrypt(pass);
			a.setPass(PASS);
			//Read line 4
			getline(file, ssn);
			string SSN = decrypt(ssn);
			a.setSSN(SSN);
			//Read line 5
			getline(file, fn);
			string FN = decrypt(fn);
			a.setFN(FN);
			//Read line 6
			getline(file, ln);
			string LN = decrypt(ln);
			a.setLN(LN);
			//Read line 7
			getline(file, org);
			string ORG = decrypt(org);
			a.setORG(ORG);
			//Read line 8
			getline(file, dob);
			string DOB = decrypt(dob);
			a.setDOB(DOB);
			//Read line 9
			getline(file, doj);
			string DOJ = decrypt(doj);
			a.setDOJ(DOJ);
			//ignore line 10;
			string ignore;
			getline(file, ignore);
			admin.push_back(a);
		}
		file.close();
	}
	return admin;
}

void signup() {
	Customer c;
	Admin a;
	int choose;
	std::cout << "1. Sign up for customer account: \n";
	std::cout << "2. Sign up for administration account: \n";
	cin >> choose;
	while (choose != 1 && choose != 2) {
		std::cout << "Reenter\n";
		std::cout << "1. Sign up for customer account: \n";
		std::cout << "2. Sign up for administration account: \n";
		cin >> choose;
	}
	long cid, aid;
	string ssn, stat;
	double money=0.0;
	string ln, fn, org, dob, doj, pass1, pass2, usn;
	std::cout << "Enter your social security number: "; cin >> ssn;
	std::cout << "Enter your first name: "; cin >> fn;
	std::cout << "Enter your last name: "; cin >> ln;
	std::cout << "Enter your Organization: "; cin >> org;
	std::cout << "Enter your Date of Birth: "; cin >> dob;
	std::cout << "Enter your Date of Join: "; cin >> doj;
	std::cout << "Enter your New Password: "; cin >> pass1;
	std::cout << "Confirm your Password: "; cin >> pass2;
	while (pass2 != pass1) {
		std::cout << "Incorrect. Reconfirm: "; cin >> pass2;
	}
	usn = username(fn, ln);
	if (choose == 1) {
		cid = rannumforid();
		//randomly generate new cid for new customers
		while (cid == c.getCID()) {
			cid = rannumforid();
		}
		std::cout << "Please remember your new username: " << usn << endl;
		std::cout << "Your Customer ID is: " << cid << endl;
		c.setCID(cid);
		c.setCB(money);
		c.setUSN(usn);
		c.setPass(pass1);
		c.setSSN(ssn);
		c.setFN(fn);
		c.setLN(ln);
		c.setORG(org);
		c.setDOB(dob);
		c.setDOJ(doj);
		c.setStat("Active");
		cus.emplace_back(cid, money, usn, pass1, ssn, fn, ln, org, dob, doj, "Active");
		generatesummaryreport(c);
	}
	else {
		aid = rannumforid();
		while (aid == a.getAID()) {
			aid = rannumforid();
		}
		std::cout << "Please remember your new username: " << usn << endl;
		std::cout << "Your Administor ID is: " << aid << endl;
		a.setAID(aid);
		a.setUSN(usn);
		a.setPass(pass1);
		a.setSSN(ssn);
		a.setFN(fn);
		a.setLN(ln);
		a.setORG(org);
		a.setDOB(dob);
		a.setDOJ(doj);
		ad.emplace_back(aid, usn, pass1, ssn, fn, ln, org, dob, doj);
		adminlist(a);
	}
	std::cout << "\nPlease log in again at Home Page!" << endl;
}

void Cus_func(Customer c) {
	read_report("account_balance_report.txt");
	int n = 0;
	if (c.getStat() != "Inactive") {
		while (n != 7) {
			std::cout << "\n1. Statement summary last N transactions" << endl;
			std::cout << "2. Current Balance" << endl;
			std::cout << "3. Withdraw" << endl;
			std::cout << "4. Deposit" << endl;
			std::cout << "5. Transfer to other CID" << endl;
			std::cout << "6. Check Inbox" << endl;
			std::cout << "7. Logout" << endl;
			std::cout << "Enter a number to continue (1-7): "; cin >> n;
			while (n < 1 || n > 7) {
				std::cout << "Invalid. Re-enter: "; cin >> n;
			}
			if (n == 1) {
				c.lastNTransactions();
			}
			else if (n == 2)
			{
				cout << "Your current balance: $" << c.getCB();
			}
			else if (n == 3)
			{
				double out;
				std::cout << "How much do you want to have? \n"; cin >> out;
				while (out <= 0) {
					std::cout << "Invalid Value. How much do you want to have? "; cin >> out;
				}
				c.withdrawal(out);
			}
			else if (n == 4)
			{
				double m;
				std::cout << "How much have you inputed? \n"; cin >> m;
				while (m <= 0) {
					std::cout << "Invalid Value. Re-enter: "; cin >> m;
				}
				c.deposit(m);
			}
			else if (n == 5)
			{
				double t;
				long to_cid;
				std::cout << "Enter the customer's cid that you want to transfer to: "; cin >> to_cid;
				std::cout << "Enter the amount of money you want to transfer: "; cin >> t;
				while (t <= 0) {
					std::cout << "Invalid value. Please Re-enter: "; cin >> t;
				}
				c.transfer(t, to_cid);
			}
			else if (n == 6) {
				c.showInboxMessages();
			}
		}
	}
	else {
		std::cout << "Please contact the Admin to unblock!" << endl;
	}
	if (n == 7) {
		std::cout << "You are being directed back to Homepage" << endl;
		return;
	}
}

bool checkcussignin(string usn, string pass) {
	bool check = false;
	vector<Customer> c = readcustextfile("users_info.txt");
	for (int i = 0; i < c.size(); i++) {
		if (usn == c[i].getUSN() && pass == c[i].getPass() && 
			c[i].countwarnings() <= 3) {
			check = true;
		}
	}
	return check;
}

bool checkadsignin(string usn, string pass) {
	bool check = false;
	vector<Admin> a = readadtextfile("List_of_Admin.txt");
	for (int i = 0; i < a.size(); i++) {
		if (usn == a[i].getUSN() && pass == a[i].getPass()) {
			check = true;
		}
	}
	return check;
}

void cus_signin() {
	string usn, pass;
	Customer c;
	std::cout << "Enter username: "; cin >> usn;
	std::cout << "Enter password: "; cin >> pass;
	bool check = checkcussignin(usn, pass);
	int n = 1;
	while (check == false && n <= 3) {
		std::cout << "Incorrect username or password or you have been receiving warning more than 3 times! Pay the debt." << endl;
		std::cout << "Re-enter username: "; cin >> usn;
		std::cout << "Re-enter password: "; cin >> pass;
		n++;
	}
	while (check == true) {
		Cus_func(c);
	}
}

void ad_func(Admin a) {
	int c = 0;
	vector<Customer>cus;
	while (c != 4) {
		std::cout << "\n1. Pay Monthly Interest" << endl;
		std::cout << "2. List of blocked accounts" << endl;
		std::cout << "3. Send warning" << endl;
		std::cout << "4. Log out" << endl;
		std::cout << "Enter an option from 1 to 4: "; cin >> c;
		while (c < 1 || c > 4) {
			std::cout << "Re-enter: "; cin >> c;
		}
		if (c == 1) {
			a.payInterest(cus);
		}
		else if (c == 2) {
			a.blockedaccounts(cus);
		}
		else if (c == 3) {
			a.sendwarning(cus);
		}
	}
	if (c == 4) {
		std::cout << "You are being directed back to Hompage!" << endl;
		return;
	}
}

void ad_signin() {
	string usn, pass;
	Admin a;
	cout << "Enter username: "; cin >> usn;
	cout << "Enter password: "; cin >> pass;
	bool check = checkadsignin(usn, pass);
	int n = 1;
	while (check == false && n <= 3) {
		std::cout << "Incorrect. Re-enter username: "; cin >> usn;
		std::cout << "Incorrect. Re-enter password: "; cin >> pass;
		n++;
	}
	while (check == true) {
		ad_func(a);
	}
}

int main() {
	int choose = 0;
	while (choose != 4) {
		std::cout << "\n**********Welcome to the Home Page of your banking account!**********" << endl;
		std::cout << "1. Admin Sign In" << endl;
		std::cout << "2. Customer Sign In" << endl;
		std::cout << "3. Sign up Page" << endl;
		std::cout << "4. Exit application" << endl;
		std::cout << "Enter your option from 1 to 4: "; cin >> choose;
		while (choose < 1 || choose > 4) {
			std::cout << "Re-enter: "; cin >> choose;
		}
		if (choose == 1) {
			readadtextfile("List_of_Admin.txt");
			ad_signin();
		}
		else if (choose == 2) {
			readcustextfile("users_info.txt");
			cus_signin();
			Customer c;
			generate_report(c);
		}
		else if (choose == 3) {
			signup();
		}
	}
	if (choose == 4) {
		std::exit(0);
	}
	return 0;
}
