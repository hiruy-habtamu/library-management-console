struct User {
    int userID;
    string first_name;
    string last_name;
    string email;
    string role;
    string password;
    string status;
    string universityID;

    // The const in the func makes sure it doesnt affect the struct instance
    void displayUserDetails() const {
        cout << "User ID: " << userID << endl;
        cout << "Name: " << first_name << " " << last_name << endl;
        cout << "Email: " << email << endl;
        cout << "Role: " << role << endl;
        cout << "University ID: " << universityID << endl;
    }

    // checks if the struct instance is empty
    bool is_empty() const {
        if (userID == 0 && first_name.empty() && last_name.empty() && email.empty() && role.empty()
            && password.empty() && status.empty() && universityID.empty()) {
            return true;
        }
        else {
            return false;
        }
    }

    // clears the user struct instance
    void clear() {
        userID = 0;
        first_name="";
        last_name="";
        email="";
        role="";
        password="";
        status="";
        universityID="";
    }
};


//Get User table GLOBALLY

mysqlx::Table userTable = db.getTable("user");

User signup() {
    //Finds the max userID
    mysqlx::RowResult rr = userTable.select("MAX(userID)").execute();

    mysqlx::Row row = rr.fetchOne();

    int last_user_id=0;
    int new_user_id=0;

    if (row) {
        last_user_id = row[0];
        new_user_id = last_user_id + 1;
    }
    else {
        new_user_id = 1;
    }
    string first_name,last_name, email, role, password,status, universityID;
    char roleChoice;

    cout << "Enter First Name: ";
    cin >> first_name;
    cout << endl;
    cout << "Enter Last Name: ";
    cin >> last_name;
    cout << endl;
    // Get unique email
    while (true) {
        cout << "Enter email address : ";
        cin >> email;

        rr = userTable.select("UserID").where("Email = :email ").bind("email", email).execute();

        if (rr.count() > 0) {
            cout << "Email already exists!" << endl;
        }
        else {
            break;
        }
    }
    cout << endl;

    // Force Roles
    cout << "Select Role (Default =  Student):"<< endl <<"1. Student"<<endl<<"2. Faculty"<<endl<<"3. Librarian \n";
    cin >> roleChoice;
    
    switch (roleChoice) {
    case '1':
        role = "Student";
        break;
    case '2':
        role = "Faculty";
        break;
    case '3':
        role = "Librarian";
        break;
    default:
        role = "Student";
    }
    status = "Active";
    
    cout << "Enter Password: ";
    
    while(true){
    cin >> password;

    if (password.length() < 8) {
        cout << "Password must be at least 8 characters long" << endl;
        continue;
    }
    else {
        bool has_special=false;
        bool has_digit = false;
        for (char a : password) {
            if (ispunct(a)) {
                has_special = true;
            }
			if (isdigit(a)) {
				has_digit = true;
            }
        }
        if (!has_special) {
            cout << "Password must contain at least one special character" << endl;
            continue;
        }
        if (!has_digit) {
            cout << "Password must contain at least one digit" << endl;
            continue;
        }
     break;
        }
    }
    
    // Hash password using picosha2 header

    string hashed_password = picosha2::hash256_hex_string(password);

    // Get unique University ID
    while (true) {
        cout << "Enter University ID: ";
        cin >> universityID;

        rr = userTable.select("UserID").where("UniversityID = :uni_id ").bind("uni_id", universityID).execute();

        if (rr.count() > 0) {
            cout << "University ID already exists!" << endl;
        }
        else {
            break;
        }
    }

    // Try pushing user data to database
    try
    {
        userTable.insert("UserID", "FirstName", "LastName", "Email", "Role", "Password", "Status", "UniversityID")
                 .values(new_user_id, first_name, last_name, email, role, hashed_password, status, universityID).execute();

        cout << "User registered successfully!" << endl;

        User newUser = {new_user_id,first_name,last_name,email,role,hashed_password,status,universityID };

        return newUser;
    }
    catch (const mysqlx::Error &err)
    {
        cout << "ERROR CREATING ACCOUNT: " << err.what();
    }
    
    //returns empty user if there is error in user creation
    User newUser;
    return newUser;
}

User login() {
    // Create empty user structure

    User user;

    string email, plain_password,password;
    int tries = 4;
    
    cout << "Enter email: ";
    cin >> email;
    cout << endl;
    while(tries > 0){
        
        cout << "Enter Password: ";
        cin >> plain_password;

        password = picosha2::hash256_hex_string(plain_password);
        
        mysqlx::RowResult rr = userTable.select("Password").where("Email = :email").bind("email",email).execute();
        mysqlx::Row row = rr.fetchOne();



        if (!row.isNull()) {
            // row[0] returns a mysqlx::value type so it needs to be converted

            string query_password = row[0].get<string>();

            if (query_password == password) {
                rr = userTable.select("*").where("Email = :email").bind("email", email).execute();
                row = rr.fetchOne();
                
                user.userID=row[0].get<int>();
                user.first_name=row[1].get<string>();
                user.last_name=row[2].get<string>();
                user.email=row[3].get<string>();
                user.role=row[4].get<string>();
                user.password=row[5].get<string>();
                user.status=row[6].get<string>();
                user.universityID=row[7].get<string>();
                
                // Check if user is inactive then ask to make it active
                if (user.status == "Inactive") {
                    char activate_choice;
                    cout << "Your account seems to be inactive\n1.Activate\n2.Exit (Other input taken as exit)";
                    cin >> activate_choice;
                    switch (activate_choice) {
                    case '1':
                        // Activate user in database
                        userTable.update().set("Status","Active").where("UserID = :id").bind("id",user.userID).execute();
                        
                        // Set current struct status to active
                        user.status = "Active";
                        system("cls");
                        return user;
                        break;
                    case '2':
                        user.clear();
                        system("cls");
                        return user;
                        break;
                    default:
                        user.clear();
                        system("cls");
                        return user;
                        break;
                    }
                }
                else {
                    return user;
                }

            }
            else {
                system("cls");
                cout << "Password doesnt match!" << endl;
                tries--;
                cout << "You have " << tries << " tries left!!" << endl;
                continue;
            }
        }
        else {
            system("cls");
            cout << "Email not found!" << endl;
            user.clear();
            return user;
        }
        
    }
    system("cls");
    user.clear();
    return user;
}