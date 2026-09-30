#include <iostream>
#include "vault.h"
#include "fileUtil.h"
#include "fileVault.h"
// #include "fileRecord.h"

void runPasswordMenu(Vault &vault);
void runFileMenu(FileVault &drive);

int main() {
    Vault myVault;
    FileVault myDrive;
    myVault.loadFromFile();
    myDrive.loadFromFile();

    // int choice;

    while(true) {
        std::cout << "=== ToyseDrive Console ===\n";
        std::cout << "1. Password Vault\n";
        std::cout << "2. File Drive\n";
        std::cout << "3. Exit\n";
        std::cout << "Choice: ";

        int mainChoice;
        std::cin >> mainChoice;

        if(mainChoice == 1) runPasswordMenu(myVault);
        else if(mainChoice == 2) runFileMenu(myDrive);
        else break;
    }
}

void runPasswordMenu(Vault &vault) {
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

                
                vault.add(Credential(s, u, p));
                vault.saveToFile();

                std::cout << "Credentials Added!\n";
                std::cout << "-------------------------" << std::endl;
                std::cout << std::endl;

                break;
            }
            
            case 2: {
                vault.listAll();

                std::cout << "Vault size: " << vault.size() << std::endl;
                std::cout << std::endl;

                break;
            }

            case 3: {
                std::string searchTerm;

                std::cout << "Enter site name to search: "; std::cin >> searchTerm;
                vault.searchBySite(searchTerm);

                std::cout << std::endl;

                break;
            }
            
            case 4: {
            std::string deleteTerm;

            std::cout << "Enter word to delete: ";
            std::cin >> deleteTerm;

            vault.deleteByUserName(deleteTerm);
                std::cout << "Credentials deleted" << std::endl;
                std::cout << std::endl;

                break;
            }

            case 5: {
                std::cout << "Goodbye!!" << std::endl;
                return ;

                break;
            }

            default:
                std::cout << "Invalid choice" << std::endl;
        }
    } while (choice != 5);
}

void runFileMenu(FileVault &drive) {
    int choice;

    do {
        std::cout << "---- DRIVE MENU LOOP ----" << std::endl;
        std::cout << "1. Upload" << std::endl;
        std::cout << "2. List" << std::endl;
        std::cout << "3. Download by ID" << std::endl;
        std::cout << "4. Delete by ID" << std::endl;
        std::cout << "5. Back" << std::endl;

        std::cout << std::endl;

        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string inputPath;
                std::cout << "Enter file path to upload: ";
                std::cin.ignore();
                std::getline(std::cin, inputPath);

                std::string rawData = FileUtil::readBinary(inputPath);
                if(rawData.empty()) {
                    std::cout << "Failed to read\n";
                    break;
                }

                std::string newId = FileUtil::generateId(8);
                std::string fileName = inputPath;
                size_t pos = inputPath.find_last_of("/\\");
                if(pos != std::string::npos) fileName = inputPath.substr(pos+1);

                std::string encoded = FileUtil::base64_encode(rawData);
                FileRecord record(newId, fileName, encoded);
                drive.add(record);
                drive.saveToFile();

                std::cout << "Uploaded! ID: " << newId << " (" << rawData.size() << " bytes)\n";
                break;
            }
            case 2: {
                drive.listAll();
                std::cout << "Total: " << drive.size() << std::endl;
                break;
            }
            case 3: {
                std::string id, outPath;
                std::cout << "Enter ID: ";
                std::cin >> id;
                std::cout << "Save as: ";
                std::cin.ignore();
                std::getline(std::cin, outPath);

                FileRecord* found = drive.findById(id);
                if(!found) {
                    std::cout << "Not found\n";
                    break;
                }

                std::string decoded = FileUtil::base64_decode(found->getData());
                if(FileUtil::writeBinary(outPath, decoded)) {
                    std::cout << "Saved to " << outPath << std::endl;
                } 
                break;
            }
            case 4: {
                std::string id;
                std::cout << "Enter ID to delete: ";
                std::cin >> id;
                if(drive.deleteById(id)) {
                    drive.saveToFile();
                    std::cout << "Deleted\n";
                } else {
                    std::cout << "ID not found\n";
                }
                break;
            }
            case 5: {
                return;
            }

            default:
                std::cout << "Invalid choice\n";
        }

    } while (choice != 5);
    
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