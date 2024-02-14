#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

class ContactDetails
{
public:
    std::string phoneNumber;
    std::string email;

    ContactDetails(const std::string& phoneNumber, const std::string& email)
        : phoneNumber(phoneNumber), email(email) {}
};

class TrieNode
{
public:
    static int nextId;
    int id;
    std::map<char, TrieNode*> children;
    ContactDetails* details;

    TrieNode() : id(-1), details(nullptr) {}
    void setId()
    {
        this->id = ++nextId;
    }

    ~TrieNode()
    {
        delete details;
        for (const auto& entry : children)
        {
            delete entry.second;
        }
    }
};

class Print
{
public:
    static void userAdded(const std::string& name)
    {
        std::cout << "Contact '" << name << "' added successfully." << std::endl;
    }
    static void userNotAdded(const std::string& name)
    {
        std::cout << "User '" << name << "' already exists." << std::endl;
    }
    static void contact(TrieNode* node, const std::string& name)
    {
        if (node && node->details)
        {
            std::cout << "ID: " << node->id << ", Name: " << name << ", Phone: " << node->details->phoneNumber
                      << ", Email: " << node->details->email << std::endl;
        }
        else
        {
            std::cout << "Contact '" << name << "' not found." << std::endl;
        }
    }
};

int TrieNode::nextId = 0;

class PhoneBook
{
private:
    TrieNode* root;
    std::unordered_map<int, TrieNode*> idToNodeMap;

    bool isValidPhoneNumber(const std::string& phoneNumber)
    {
        // Simple validation: Check if the phone number has at least 10 digits
        return phoneNumber.length() >= 10;
    }

    bool isValidEmail(const std::string& email)
    {
        // Simple validation: Check if the email contains '@'
        return email.find('@') != std::string::npos;
    }

    bool insertContact(TrieNode* node, const std::string& name, const ContactDetails& details, size_t index)
    {
        if (index == name.length())
        {
            if (!node->details)
            {
                node->details = new ContactDetails(details);
                node->setId();
                idToNodeMap[node->id] = node;
                return true;
            }
            return false;
        }

        char currentChar = name[index];
        if (node->children.find(currentChar) == node->children.end())
        {
            node->children[currentChar] = new TrieNode();
        }

        return insertContact(node->children[currentChar], name, details, index + 1);
    }

    TrieNode* searchContact(TrieNode* node, const std::string& name, size_t index)
    {
        if (index == name.length())
        {
            return node;
        }

        char currentChar = name[index];
        if (node->children.find(currentChar) != node->children.end())
        {
            return searchContact(node->children[currentChar], name, index + 1);
        }

        return nullptr;
    }

    TrieNode* searchContactByEmail(TrieNode* node, const std::string& email, const std::string& name = "",
                                   bool exactMatch = true)
    {
        if (node && node->details && node->details->email == email)
        {
            if (!exactMatch || (exactMatch && name == ""))
            {
                return node;
            }
        }

        for (const auto& entry : node->children)
        {
            TrieNode* result = searchContactByEmail(entry.second, email, name, exactMatch);
            if (result)
            {
                return result;
            }
        }

        return nullptr;
    }

    TrieNode* searchContactByPhoneNumber(TrieNode* node, const std::string& phoneNumber, const std::string& name = "",
                                         bool exactMatch = true)
    {
        if (node && node->details && node->details->phoneNumber == phoneNumber)
        {
            if (!exactMatch || (exactMatch && name == ""))
            {
                return node;
            }
        }

        for (const auto& entry : node->children)
        {
            TrieNode* result = searchContactByPhoneNumber(entry.second, phoneNumber, name, exactMatch);
            if (result)
            {
                return result;
            }
        }

        return nullptr;
    }

public:
    PhoneBook() : root(new TrieNode()) {}
    ~PhoneBook()
    {
        delete root;
    }

    bool insertContact(const std::string& name, const std::string& phoneNumber, const std::string& email)
    {
        if (!isValidPhoneNumber(phoneNumber) || !isValidEmail(email))
        {
            return false;
        }

        ContactDetails details(phoneNumber, email);
        return insertContact(root, name, details, 0);
    }

    TrieNode* searchContactByName(const std::string& name)
    {
        return searchContact(root, name, 0);
    }

    TrieNode* searchContactByEmail(const std::string& email, bool exactMatch = true)
    {
        return searchContactByEmail(root, email, "", exactMatch);
    }

    TrieNode* searchContactByPhoneNumber(const std::string& phoneNumber, bool exactMatch = true)
    {
        return searchContactByPhoneNumber(root, phoneNumber, "", exactMatch);
    }

    bool editContact(const std::string& name, const std::string& newPhoneNumber, const std::string& newEmail)
    {
        TrieNode* node = searchContactByName(name);
        if (node && node->details)
        {
            if (isValidPhoneNumber(newPhoneNumber))
            {
                node->details->phoneNumber = newPhoneNumber;
            }
            if (isValidEmail(newEmail))
            {
                node->details->email = newEmail;
            }
            return true;
        }
        return false;
    }

    bool deleteContact(const std::string& name)
    {
        TrieNode* node = searchContactByName(name);
        if (node && node->details)
        {
            delete node->details;
            node->details = nullptr;
            return true;
        }
        return false;
    }

    void displayAllContacts()
    {
        if (root->children.empty())
        {
            std::cout << "No contacts found." << std::endl;
            return;
        }

        displayAllContacts(root, "");
    }

    TrieNode* searchContactById(int id)
    {
        if (idToNodeMap.find(id) != idToNodeMap.end())
        {
            return idToNodeMap[id];
        }
        return nullptr;
    }

private:
    void displayAllContacts(TrieNode* node, const std::string& prefix)
    {
        if (node && node->details)
        {
            Print::contact(node, prefix);
        }

        for (const auto& entry : node->children)
        {
            displayAllContacts(entry.second, prefix + entry.first);
        }
    }
};

/*int main()
{
    PhoneBook phoneBook;

    phoneBook.insertContact("John Doe", "1234567890", "john.doe@example.com");
    phoneBook.insertContact("Jane Smith", "9876543210", "jane.smith@example.com");
    phoneBook.insertContact("Alice Johnson", "5555555555", "alice.johnson@example.com");

    phoneBook.displayAllContacts();

    TrieNode* foundContact = phoneBook.searchContactByName("Jane Smith");
    if (foundContact)
    {
        Print::contact(foundContact, "Jane Smith");
    }

    return 0;
}*/int main()
{
    PhoneBook phoneBook;

    while (true)
    {
        std::cout << "Phone Book Menu:" << std::endl;
        std::cout << "1. Add Contact" << std::endl;
        std::cout << "2. Search Contact by Name" << std::endl;
        std::cout << "3. Search Contact by Email" << std::endl;
        std::cout << "4. Search Contact by Phone Number" << std::endl;
        std::cout << "5. Edit Contact" << std::endl;
        std::cout << "6. Delete Contact" << std::endl;
        std::cout << "7. Display All Contacts" << std::endl;
        std::cout << "8. Search Contact by ID" << std::endl;
        std::cout << "9. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice)
        {
            case 1:
            {
                std::string name, phoneNumber, email;
                std::cout << "Enter name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter phone number: ";
                std::getline(std::cin, phoneNumber);
                std::cout << "Enter email: ";
                std::getline(std::cin, email);

                if (phoneBook.insertContact(name, phoneNumber, email))
                {
                    Print::userAdded(name);
                }
                else
                {
                    Print::userNotAdded(name);
                }
                break;
            }
            case 2:
            {
                std::string name;
                std::cout << "Enter name: ";
                std::cin.ignore();
                std::getline(std::cin, name);

                TrieNode* foundContact = phoneBook.searchContactByName(name);
                Print::contact(foundContact, name);
                break;
            }
            case 3:
            {
                std::string email;
                std::cout << "Enter email: ";
                std::cin.ignore();
                std::getline(std::cin, email);

                TrieNode* foundContact = phoneBook.searchContactByEmail(email);
                Print::contact(foundContact, "");
                break;
            }
            case 4:
            {
                std::string phoneNumber;
                std::cout << "Enter phone number: ";
                std::cin.ignore();
                std::getline(std::cin, phoneNumber);

                TrieNode* foundContact = phoneBook.searchContactByPhoneNumber(phoneNumber);
                Print::contact(foundContact, "");
                break;
            }
            case 5:
            {
                std::string name, newPhoneNumber, newEmail;
                std::cout << "Enter name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter new phone number: ";
                std::getline(std::cin, newPhoneNumber);
                std::cout << "Enter new email: ";
                std::getline(std::cin, newEmail);

                if (phoneBook.editContact(name, newPhoneNumber, newEmail))
                {
                    std::cout << "Contact '" << name << "' updated successfully." << std::endl;
                }
                else
                {
                    std::cout << "Contact '" << name << "' not found." << std::endl;
                }
                break;
            }
            case 6:
            {
                std::string name;
                std::cout << "Enter name: ";
                std::cin.ignore();
                std::getline(std::cin, name);

                if (phoneBook.deleteContact(name))
                {
                    std::cout << "Contact '" << name << "' deleted successfully." << std::endl;
                }
                else
                {
                    std::cout << "Contact '" << name << "' not found." << std::endl;
                }
                break;
            }
            case 7:
            {
                phoneBook.displayAllContacts();
                break;
            }
            case 8:
            {
                int id;
                std::cout << "Enter ID: ";
                std::cin >> id;

                TrieNode* foundContact = phoneBook.searchContactById(id);
                Print::contact(foundContact, "");
                break;
            }
            case 9:
                std::cout << "Exiting..." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }

        std::cout << std::endl;
    }
}
