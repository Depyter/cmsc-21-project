#include <iostream>
#include <string>

// Function to encrypt the string
std::string encrypt(const std::string& text, int shift) {
    std::string result = text;
    for (int i = 0; i < text.length(); ++i) {
        if (isupper(text[i])) {
            result[i] = char(int((text[i] - 'A' + shift) % 26 + 'A'));
        } else if (islower(text[i])) {
            result[i] = char(int((text[i] - 'a' + shift) % 26 + 'a'));
        }
    }
    return result;
}

// Function to decrypt the string
std::string decrypt(const std::string& text, int shift) {
    std::string result = text;
    for (int i = 0; i < text.length(); ++i) {
        if (isupper(text[i])) {
            result[i] = char(int((text[i] - 'A' - shift + 26) % 26 + 'A'));
        } else if (islower(text[i])) {
            result[i] = char(int((text[i] - 'a' - shift + 26) % 26 + 'a'));
        }
    }
    return result;
}

int main() {
    std::string text;
    int shift;

    std::cout << "Enter the text to be encrypted: ";
    std::getline(std::cin, text);

    std::cout << "Enter the shift value: ";
    std::cin >> shift;

    std::string encryptedText = encrypt(text, shift);
    std::string decryptedText = decrypt(encryptedText, shift);

    std::cout << "Encrypted Text: " << encryptedText << std::endl;
    std::cout << "Decrypted Text: " << decryptedText << std::endl;

    return 0;
}
