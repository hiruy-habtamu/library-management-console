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
