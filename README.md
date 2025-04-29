# LIBRARY MANAGEMENT CONSOLE APPLICATION

This repository contains a C++ console application that connects to MySQL using the MySQL Connector/C++ with the X DevAPI.

## Requirements

Before you begin, ensure you have the following installed:

- **Visual Studio 2022** (with the Desktop development with C++ workload installed)
- **MySQL Connector/C++ 9.3** (includes the X DevAPI)


## Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/hiruy-habtamu/library-management-console.git
cd library-management-console
```

### 2. Install MySQL Connector/C++

You can download MySQL Connector/C++ from the official MySQL website:

[Download MySQL Connector/C++](https://dev.mysql.com/downloads/connector/cpp/)

Follow the installation instructions and install the connector.

Make note of the installation path (e.g., `C:\Program Files\MySQL\MySQL Connector C++ 9.3\`).

### 3. Open the Project in Visual Studio

Open the `library-management-console.sln` file with Visual Studio.

### 4. Configure the Project in Visual Studio

#### Add Include and Library Directories:


1. Right-click the project and select **Properties**.
2. Under **Configuration Properties**, go to **VC++ Directories**.
   - **Include Directories**: Overwrite  the path to MySQL Connector/C++ headers: `$(WindowsSDK_IncludePath);$(IncludePath);C:\Program Files\MySQL\MySQL Connector C++ 9.3\include;$(VC_IncludePath);`
   - **Library Directories**: Overwrite the path to the `lib` folder: `$(VC_LibraryPath_x64);$(WindowsSDK_LibraryPath_x64);C:\Program Files\MySQL\MySQL Connector C++ 9.3\lib64\vs14`
  
#### Add Additional Include Directories:
1. Under **Configuration Properties**, go to **C/C++ -> General -> Additional Include Directories**.
2. Overwrite to `C:\Program Files\MySQL\MySQL Connector C++ 9.3\include\mysqlx`

### Configure Preprocessors
1. Under **Configuration Properties**, go to **C/C++ -> General -> Preprocessor Definiitons**.
2. Overwrite to `_CRT_SECURE_NO_WARNINGS;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)`

#### Link the Required Libraries:

1. Go to **Configuration Properties → Linker → Input → Additional Dependencies**.
2. Overwrite to `$(CoreLibraryDependencies);%(AdditionalDependencies);mysqlcppconnx.lib;`.

#### Copy DLL Files:

Copy the `mysqlcppconnx-2-vs14.dll` file to the project directory where the `.exe` is found which is in `x64/Release` or `x64/Debug` folder. If it does not exist create the folder.

### 5. Build and Run the Project

- Build the project by pressing `Ctrl + Shift + B`.
- To run the project, press `Ctrl + F5` or `F5`.

### 6. Sample Code: Connecting to MySQL Database

Here’s a sample C++ code using the X DevAPI to connect to a MySQL server:

```cpp
#include <iostream>
#include <mysqlx/xdevapi.h>

int main() {
    try {
        // Create a session
        mysqlx::Session sess(
            "localhost",    // hostname
            33060,          // X Protocol port (NOT 3306)
            "root",         // username
            "root" // password
        );

        std::cout << "✅ Successfully connected to MySQL Server using X DevAPI!" << std::endl;

        // Get list of schemas (returns Schema objects)
        auto schemas = sess.getSchemas();
        std::cout << "Available Schemas:" << std::endl;
        for (const auto& schema : schemas) {
            std::cout << " - " << schema.getName() << std::endl;
        }

        // Close the session
        sess.close();
        std::cout << "✅ Session closed." << std::endl;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "❌ MySQL Error: " << err.what() << std::endl;
        return 1;
    }
    catch (std::exception& ex) {
        std::cerr << "❌ STD Exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "❌ Unknown Exception caught!" << std::endl;
        return 1;
    }

    return 0;
}

```

### 7. Troubleshooting

- **Error: "cannot open input file 'mysqlcppconnx.lib'"**:
  - Ensure the `mysqlcppconnx.lib` file is correctly linked in the project settings under **Linker > Input > Additional Dependencies**.

- **Error: "Cannot open mysqlcppconnx-2-vs14.dll"**:
  - Make sure the `mysqlcppconnx-2-vs14.dll` file is either in the project directory or in a directory that's included in your system's PATH environment variable.

- **Unable to connect to MySQL server**:
  - Verify that the MySQL server is running on the correct host and port.
  - Check that the credentials (username and password) are correct.

### 8. Notes

- The X DevAPI works with MySQL 8.0 and above. Ensure that your MySQL server is configured for X Protocol and is running on port 33060 by default.
- MySQL Connector/C++ 8.0 or later is required for using the X DevAPI. Make sure you're using the correct version.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
