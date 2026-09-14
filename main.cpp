#include <iostream>
#include "vault.h"

int main() {
    Vault myVault;
    myVault.loadFromFile();

    int choice;

    do {
        std::cout << "---- MENU LOOP ----" << std::endl;
        std::cout << "1. Add" << std::endl;
        std::cout << "2. List" << std::endl;
        std::cout << "3. Search" << std::endl;
        std::cout << "4. Delete" << std::endl;
        std::cout << "5. Exit" << std::endl;

        std::cout << std::endl;

        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string s, u, p;
 
                std::cout << "Enter site name: "; std::cin >> s;
                std::cout << "Enter username: "; std::cin >> u;
                std::cout << "Enter password: "; std::cin >> p;

                
                myVault.add(Credential(s, u, p));
                myVault.saveToFile();

                std::cout << "Credentials Added!\n";
                std::cout << "-------------------------" << std::endl;
                std::cout << std::endl;

                break;
            }
            
            case 2: {
                myVault.listAll();

                std::cout << "Vault size: " << myVault.size() << std::endl;
                std::cout << std::endl;

                break;
            }

            case 3: {
                std::string searchTerm;

                std::cout << "Enter site name to search: "; std::cin >> searchTerm;
                myVault.searchBySite(searchTerm);

                std::cout << std::endl;

                break;
            }
            
            case 4: {
            std::string deleteTerm;

            std::cout << "Enter word to delete: ";
            std::cin >> deleteTerm;

            myVault.deleteByUserName(deleteTerm);
                std::cout << "Credentials deleted" << std::endl;
                std::cout << std::endl;

                break;
            }

            case 5: {
                std::cout << "Goodbye!!" << std::endl;
                return 0;

                break;
            }

            default:
                std::cout << "Invalid choice" << std::endl;
        }
    } while (choice != 5);

    return 0;
}

// QUESTIONS:
// Ho do I prevent duplicate sites?
// Answer: The best way is to check if the site already exists in the vault::add method before adding a new credentials.
// Example: bool add(const Credential& c) {
//     for (const auto& item : items) {
//         if (item.getSite() == c.getSite() & item.getUser() == c.getUser()) {
//             std::cout << "Credential already exists." << std::endl;
//             return false;
//         }
//     };
//     items.push_back(c);
//     return true;
// } 


// What happens if vault.txt is corrupted?
// Answer: The program will crash if the file is corrupted. To prevent this, we can add error handlers for example, using npos
// Example: if (pos1 == std::string::npos || pos2 == std::string::npos) {
//     std::cout << "Skipping corrupted line: " << line << std::endl;
// }


// Should Vault be responsible for file I/O or should I create a separate FileManager class? Why?
// Answer: Vault should NOT handle I/O for clarity. This is called Single Responsibility Principle. Right now Vault does two jobs: holds data AND saves it. 
// In a large program it would be split: FileManager::save(items, fileName) and FileManager::load(fileName) -> vector<Credential>