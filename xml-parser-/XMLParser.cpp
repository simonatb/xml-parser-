#include "XMLParser.h"

// Helper functions
char digitToChar(int digit) {
    if (digit >= 0 && digit <= 9) {
        return '0' + digit;
    }
    return '\0';
}

int numberSize(int number) {
    if (number == 0) return 1;
    int length = 0;
    while (number) {
        number /= 10;
        length++;
    }
    return length;
}

String fromIntegerToString(int number) {
    if (number == 0) return "0";

    String result;

    int length = numberSize(number);
    for (int i = 0; i < length; i++) {
        int divisor = 1;
        for (int j = 0; j < length - i - 1; j++) {
            divisor *= 10;
        }

        int digit = number / divisor;
        result += digitToChar(digit);
        number %= divisor;
    }

    return result;
}

void skipWhitespace(String& xml, size_t& pos) {
    while (pos < xml.length() && xml.isSpace(pos)) {
        ++pos;
    }
}

unsigned charSize(const char* first) {
    if (first == nullptr) {
        return 0;
    }

    int length = 0;
    while (*first != '\0') {
        first++;
        length++;
    }
    return length;
}


bool startsWith(const char* text, const char* pattern) {
    if (!text || !pattern) {
        return false;
    }

    while (*text != '\0' && *pattern != '\0') {
        if (*text != *pattern) {
            return false;
        }
        text++;
        pattern++;
    }
    return *pattern == '\0';
}

bool isSubstring(const char* text, const char* pattern) {
    if (!text || !pattern) {
        return false;
    }

    unsigned textLength = charSize(text);
    unsigned patternLength = charSize(pattern);

    while (textLength >= patternLength) {
        if (startsWith(text, pattern)) {
            return true;
        }
        text++;
        textLength--;
    }
    return false;
}

Element* XMLParser::parseXML(const String& filename) {
    String buffer;
    buffer.readFile(filename.c_str());

    Element* root = nullptr;
    Element* current = nullptr;
    size_t pos = 0;
    while (pos < buffer.length()) {
        skipWhitespace(buffer, pos);

        if (buffer[pos] == '<' && buffer[pos + 1] != '/') {
            Element* element = parseElement(buffer, pos);
            if (element) {
                if (!root) {
                    root = element;                   
                }
                else if (!current) {
                    current = element;
                    root->setNextSibling(current);
                }
                else {

                    current->setNextSibling(element); 
                    current = element;

                }
            }
        }
        else {
            ++pos;
        }
    }
    setUniqueId(root);
    return root;
}

Element* XMLParser::parseElement(String& buffer, size_t& pos) {
    skipWhitespace(buffer, pos);
    ++pos;

    String name = parseName(buffer, pos);
    Element* element = new Element(name);

    parseAttributes(element, buffer, pos);

    if (buffer[pos] == '/' && buffer[pos + 1] == '>') {
        pos += 2;
        return element;
    }

    ++pos; 

    while (pos < buffer.length() && !(buffer[pos] == '<' && buffer[pos + 1] == '/')) {
        if (buffer[pos] == '<') {
            Element* child = parseElement(buffer, pos);
            if (child) {
                element->addChild(child);
            }
        }
        else {
            Text* text = parseText(buffer, pos);
            if (text) {
                element->addChild(text);
            }
        }
        skipWhitespace(buffer, pos);
    }

    pos += 2 + name.length() + 1;//skipvame zatvarqshiq tag s imeto

    return element;
}

void XMLParser::parseAttributes(Element* element, String& buffer, size_t& pos) {
    skipWhitespace(buffer, pos);
    while (buffer[pos] != '>' && buffer[pos] != '/') {
        String name = parseName(buffer, pos);
        skipWhitespace(buffer, pos);
        ++pos; //skipvame ravnoto
        
        char quote = buffer[pos++];
        size_t start = pos;
        while (buffer[pos] != quote) {
            ++pos;
        }

        String value = buffer.substr(start, pos - start);
        ++pos;
        Attribute* attribute = new Attribute(name, value);
        element->addAttribute(attribute);
        skipWhitespace(buffer, pos);
    }
}

Text* XMLParser::parseText(String& buffer, size_t& pos) {
    size_t start = pos;
    while (pos < buffer.length() && buffer[pos] != '<') {
        if (buffer[pos] == '\n') {
            return nullptr;
        }
        ++pos;
    }
    String text = buffer.substr(start, pos - start);
    
    if (text != "\n") {
        return new Text(text);
    }
    
}

String XMLParser::parseName(String& buffer, size_t& pos) {
    size_t start = pos;
    while (pos < buffer.length() && (buffer.isAlnum(pos))) {
        ++pos;
    }
    return buffer.substr(start, pos - start);
}

void XMLParser::print(Element* element) {
    
    while (element) {
        std::cout << "Element: " << element->getName() << std::endl;

        Attribute* attr = element->getFirstAttribute();
        while (attr) {
            std::cout << "  Attribute: " << attr->getName() << " = " << attr->getValue() << std::endl;
            attr = attr->getNext();
        }

        Node* child = element->getFirstChild();
        while (child) {
            Element* childElement = dynamic_cast<Element*>(child);
            if (childElement) {
                print(childElement);
                break;
            }
            else {
                Text* textNode = dynamic_cast<Text*>(child);
                if (textNode) {
                    std::cout << "  Text: " << textNode->getText() << std::endl;
                }
            }
            if (child->getNextSibling()) {           
                child = child->getNextSibling();
            }
            else {
                child = nullptr;
            }
        }
        if (element->getNextSibling()) {
            element = dynamic_cast<Element*>(element->getNextSibling());
        }
        else
        {
            element=nullptr;
        }
    }
    std::cout << std::endl;
}

bool XMLParser::checkId(Element* element) const
{
    String id("id");
    if (!element) {
        return 0;
    }
    Attribute* attribute = dynamic_cast<Attribute*>(element->getFirstAttribute());
    while (attribute) {
        if (attribute->getName() == id) {
            return 1;
        }
        attribute = attribute->getNext();
    }
    return 0;
}

void XMLParser::ensureId(Element* root)
{
    String idNum("0");
    String id("id");
    Element* current = root;
    while (current) {
        if (checkId(current)==0) {
            Attribute* idAttribute = new Attribute(id, idNum);
            current->addAttribute(idAttribute);
        }
        current = dynamic_cast<Element*>(current->getNextSibling());
    }
}

void XMLParser::setUniqueId(Element* root)
{
    ensureId(root);
    String id("id");
    int count = 0;
    Element* current = root;
    while (current) {
        Attribute* attribute = current->getFirstAttribute();
        while (attribute) {
            if (attribute->getName() == id) {
                attribute->setValue(fromIntegerToString(count));
                count++;
            }
            attribute = attribute->getNext();
        }
        current = dynamic_cast<Element*>(current->getNextSibling());
    }
}
