#include "NewAVL.h"

AVL_Node* AVL::rotationCaller(AVL_Node* parent) {
    if (getBF(parent) == 2)  // Case: Left rotation
    {
        if (getBF(parent->left) == -1)
            return LRrotation(parent);
        else
            return LLrotation(parent);
    }
    else if (getBF(parent) == -2) {  // Case: Right rotation
        if (getBF(parent->right) == 1)
            return RLrotation(parent);
        else
            return RRrotation(parent);
    }
    return parent;
}

AVL_Node* AVL::LLrotation(AVL_Node* parent) {
    // New variables for easier readability:
    AVL_Node* oldTop;
    AVL_Node* newTop;

    // Moving the values to the variables:
    oldTop = parent;
    newTop = oldTop->left;

    // LL Rotating:
    oldTop->left = newTop->right;
    newTop->right = oldTop;

    // Adjusting height:
    oldTop->Height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
    newTop->Height = getMax(getHeight(newTop->left), getHeight(newTop->right)) + 1;

    // Adjusting subnode & subgrade count:
    oldTop->SubNodes -= newTop->SubNodes;
    oldTop->SubGrades -= newTop->SubGrades;

    return newTop;
}

AVL_Node* AVL::LRrotation(AVL_Node* parent) {
    // New variables for easier readability:
    AVL_Node* oldTop;     // Now new left
    AVL_Node* oldMiddle;  // Now new right
    AVL_Node* oldBottom;  // now new top

    // Moving the values to the variables:
    oldTop = parent;
    oldMiddle = oldTop->left;
    oldBottom = oldMiddle->right;

    // RL Rotating
    oldTop->left = oldBottom->right;
    oldMiddle->right = oldBottom->left;
    oldBottom->right = oldTop;
    oldBottom->left = oldMiddle;

    // Adjusting height:
    oldTop->Height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
    oldMiddle->Height = getMax(getHeight(oldMiddle->left), getHeight(oldMiddle->right)) + 1;
    oldBottom->Height = getMax(getHeight(oldBottom->left), getHeight(oldBottom->right)) + 1;

    // Adjusting subnode count:
    oldBottom->SubNodes += oldMiddle->SubNodes;
    oldBottom->SubGrades += oldMiddle->SubGrades;
    oldTop->SubNodes -= oldBottom->SubNodes;
    oldTop->SubGrades -= oldBottom->SubGrades;

    return oldBottom;
}

AVL_Node* AVL::RLrotation(AVL_Node* parent) {
    // New variables for easier readability:
    AVL_Node* oldTop;     // Now new left
    AVL_Node* oldMiddle;  // Now new right
    AVL_Node* oldBottom;  // now new top

    // Moving the values to the variables:
    oldTop = parent;
    oldMiddle = oldTop->right;
    oldBottom = oldMiddle->left;

    // RL Rotating
    oldTop->right = oldBottom->left;
    oldMiddle->left = oldBottom->right;
    oldBottom->left = oldTop;
    oldBottom->right = oldMiddle;

    // Adjusting height:
    oldTop->Height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
    oldMiddle->Height = getMax(getHeight(oldMiddle->left), getHeight(oldMiddle->right)) + 1;
    oldBottom->Height = getMax(getHeight(oldBottom->left), getHeight(oldBottom->right)) + 1;

    // Adjusting subnode count:
    oldMiddle->SubNodes -= oldBottom->SubNodes;
    oldMiddle->SubGrades -= oldBottom->SubGrades;
    oldBottom->SubNodes += oldTop->SubNodes;
    oldBottom->SubGrades += oldTop->SubGrades;

    return oldBottom;
}

AVL_Node* AVL::RRrotation(AVL_Node* parent) {
    // New variables for easier readability:
    AVL_Node* oldTop;
    AVL_Node* newTop;

    // Moving the values to the variables:
    oldTop = parent;
    newTop = oldTop->right;

    // RR Rotating
    oldTop->right = newTop->left;
    newTop->left = oldTop;

    // Adjusting height:
    oldTop->Height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
    newTop->Height = getMax(getHeight(newTop->left), getHeight(newTop->right)) + 1;

    // Adjusting subnode count:
    newTop->SubNodes += oldTop->SubNodes;
    newTop->SubGrades += oldTop->SubGrades;

    return newTop;
}

int AVL::getHeight(AVL_Node* parent) {
    // Check if the node exists:
    if (parent == nullptr) return 0;

    // Return the node height:
    return parent->Height;
}

int AVL::getMax(int in_a, int in_b) {
    if (in_a > in_b)
        return in_a;
    else
        return in_b;
}

int AVL::getBF(AVL_Node* parent) {
    if (parent == nullptr) return 0;

    return getHeight(parent->left) - getHeight(parent->right);
}

void AVL::destructorAux(AVL_Node* current) {
    if (current == nullptr) return;

    destructorAux(current->left);
    destructorAux(current->right);
    delete current;
}

bool AVL::nodeExistsAux(AVL_Node* current, int inSal, int inID) {
    // Checking if we reached the end of the tree:
    if (current == nullptr) return false;

    if (current->Salary < inSal)
        return nodeExistsAux(current->right, inSal, inID);
    else if (current->Salary > inSal)
        return nodeExistsAux(current->left, inSal, inID);
    else {
        if (current->ID > inID)
            return nodeExistsAux(current->left, inSal, inID);
        else if (current->ID < inID)
            return nodeExistsAux(current->right, inSal, inID);
        else if (current->ID == inID)
            return true;
    }

    return false;
}

AVL_Node* AVL::AddNodeAux(AVL_Node* inNode, AVL_Node* inParent, int inSal, int inID, int inGrade, Employee* inEmp) {
    // Checking if the current node is the last:
    if (inNode == nullptr) {
        // Allocating a new node:
        AVL_Node* newNode = new AVL_Node;
        if (newNode == nullptr) throw ALLOCATION_ERROR;

        // Moving the data to the new node:
        newNode->Salary = inSal;
        newNode->ID = inID;
        newNode->Grade = inGrade;
        newNode->Emp = inEmp;

        // Setting the new node:
        inNode = newNode;

        inNode->left = nullptr;
        inNode->parent = inParent;
        inNode->right = nullptr;

        inNode->Height = 1;
        inNode->SubNodes = 1;
        inNode->SubGrades = inGrade;

        TreeSize++;

        return inNode;
    }
    else {
        switch (compare(inNode->Salary, inNode->ID, inSal, inID)) {
        case KeySmaller: {
            inNode->SubNodes++;
            inNode->SubGrades += inGrade;
            inNode->left = AddNodeAux(inNode->left, inNode, inSal, inID, inGrade, inEmp);
            break;
        }
        case KeyBigger: {
            inNode->right = AddNodeAux(inNode->right, inNode, inSal, inID, inGrade, inEmp);
            break;
        }
        case BackupSmaller: {
            inNode->SubNodes++;
            inNode->SubGrades += inGrade;
            inNode->left = AddNodeAux(inNode->left, inNode, inSal, inID, inGrade, inEmp);
            break;
        }
        case BackupBigger: {
            inNode->right = AddNodeAux(inNode->right, inNode, inSal, inID, inGrade, inEmp);
            break;
        }
        case Equal:
            break;
        case NotEqual:
            break;
        }
    }

    inNode->Height = getMax(getHeight(inNode->left), getHeight(inNode->right)) + 1;

    inNode = rotationCaller(inNode);

    return inNode;
}

AVL_Node* AVL::DeleteNodeAux(AVL_Node* toDelete, int inSal, int inID, int inGrade) {
    // Check if the current node exists:
    if (toDelete == nullptr) return toDelete;

    switch (compare(toDelete->Salary, toDelete->ID, inSal, inID)) {
    case KeySmaller: {
        toDelete->SubNodes--;
        toDelete->SubGrades -= inGrade;
        toDelete->left = DeleteNodeAux(toDelete->left, inSal, inID, inGrade);
        break;
    }
    case KeyBigger:
        toDelete->right = DeleteNodeAux(toDelete->right, inSal, inID, inGrade);
        break;
    case BackupSmaller:
        toDelete->SubNodes--;
        toDelete->SubGrades -= inGrade;
        toDelete->left = DeleteNodeAux(toDelete->left, inSal, inID, inGrade);
        break;
    case BackupBigger: {
        toDelete->right = DeleteNodeAux(toDelete->right, inSal, inID, inGrade);
        break;
    }
    case Equal: {
        // Found the node:

        // Checking for a suitable replacement node:
        if (toDelete->left == nullptr || toDelete->right == nullptr) {
            // Moving a son of the deleted node to a temporary one:
            AVL_Node* temp_delete = nullptr;
            if (toDelete->left == nullptr)
                temp_delete = toDelete->right;
            else if (toDelete->right == nullptr)
                temp_delete = toDelete->left;

            // Checking if both of the sons are null:
            if (temp_delete == nullptr) {
                temp_delete = toDelete;
                delete temp_delete;  // The node does not need a replacement
                toDelete = nullptr;
            }
            else {
                *toDelete = *temp_delete;  // Moving the replacement to the deleted location &
                                           // replacing the content

                // Freeing the node that was used to replace:
                delete temp_delete;
            }

            TreeSize--;
        }
        else {
            // Looking for a suitable replacement:
            AVL_Node* temp_delete = toDelete->right;  // Moving one node to the right
            while (temp_delete->left != nullptr)      // Moving all the way to the left
                temp_delete = temp_delete->left;

            // Moving the data from the replacement node:
            toDelete->SubGrades = temp_delete->SubGrades + toDelete->SubGrades - toDelete->Grade;

            toDelete->Salary = temp_delete->Salary;
            toDelete->ID = temp_delete->ID;
            toDelete->Grade = temp_delete->Grade;
            toDelete->Emp = temp_delete->Emp;

            toDelete->right = DeleteNodeAux(toDelete->right, temp_delete->Salary, temp_delete->ID, temp_delete->Grade);
        }
    }
    case NotEqual:
        break;
    }

    // Checking if the node got completly deleted:
    if (toDelete == nullptr) return toDelete;

    toDelete->Height = getMax(getHeight(toDelete->left), getHeight(toDelete->right)) + 1;

    return rotationCaller(toDelete);
}

void AVL::inOrderAux(Employee** dataArray, AVL_Node* current, int* count) {
    if (current == nullptr) return;

    inOrderAux(dataArray, current->left, count);
    dataArray[*count] = current->Emp;
    (*count)++;
    inOrderAux(dataArray, current->right, count);
}

AVL_Node* AVL::replaceAux(Employee** data, int start, int end, AVL_Node* prev, int depth) {
    // End condition:
    if (start > end) return nullptr;

    // Get the middle and make it the root:
    int middle = (start + end) / 2;
    AVL_Node* head = new AVL_Node;
    if (head == nullptr) throw ALLOCATION_ERROR;

    head->Salary = data[middle]->getSal();
    head->ID = data[middle]->getID();
    head->Grade = data[middle]->getGrade();
    head->Emp = data[middle];

    head->parent = prev;

    if (depth == 0)
        head->SubNodes = middle;
    else
        head->SubNodes = prev->SubNodes / 2;

    // Building the left side:
    head->left = replaceAux(data, start, middle - 1, head, depth + 1);
    // Building the right side:
    head->right = replaceAux(data, middle + 1, end, head, depth + 1);

    return head;
}

int AVL::fixSubnodes(AVL_Node* node, int count) {
    if (node == NULL) {
        return 0;
    }

    count = fixSubnodes(node->left, count) + 1;

    node->SubNodes = count;

    return count + fixSubnodes(node->right, count);
}

long long AVL::fixSubgrades(AVL_Node* node, long long grade) {
    if (node == NULL) {
        return 0;
    }

    grade = fixSubgrades(node->left, grade) + node->Emp->getGrade();

    node->SubGrades = grade;

    return grade + fixSubgrades(node->right, grade);
}

int AVL::fixHeight(AVL_Node* node) {
    if (node == NULL) return 0;

    node->Height = getMax(fixHeight(node->left), fixHeight(node->right)) + 1;

    return node->Height;
}

void AVL::printBT(const std::string& prefix, const AVL_Node* node, bool isLeft) {
    if (node != nullptr) {
        std::cout << prefix;

        std::cout << (isLeft ? "|---" : "|---");

        // print the value of the node
        std::cout << "Sal = " << node->Salary << " ID = " << node->ID << " Grade = " << node->Grade << " SubGrades = " << node->SubGrades << "\n";

        // enter the next tree level - left and right branch
        printBT(prefix + (isLeft ? "|                      " : "                    "), node->left, true);
        printBT(prefix + (isLeft ? "|                      " : "                    "), node->right, false);
    }
}

AVL::AVL() {
    TreeSize = 0;
    TotalGrades = 0;
    root = nullptr;
}

AVL::~AVL() {
    // Using a recursive function that goes over the tree postorder to help deleting:
    destructorAux(root);
}

int AVL::getSize() { return TreeSize; }

long long AVL::getTotal() { return TotalGrades; }

bool AVL::nodeExists(int inSal, int inID) {
    // Using a recursive function that searches for the node in a binary algo:
    return nodeExistsAux(root, inSal, inID);
}

int AVL::getRank(int inSal, int inID) {
    // Getting to the node that holds the given key:
    AVL_Node* iter = root;
    int rank = 0;

    while (true) {
        if (iter->Salary < inSal) {
            rank += iter->SubNodes;
            iter = iter->right;
        }
        else if (iter->Salary > inSal) {
            iter = iter->left;
        }
        else {
            if (iter->ID > inID) {
                iter = iter->left;
            }
            else if (iter->ID < inID) {
                rank += iter->SubNodes;
                iter = iter->right;
            }
            else if (iter->ID == inID)
                break;
        }
    }

    return rank + iter->SubNodes;
}

long long AVL::getLowerSum(int inSal, int inID) {
    // Getting to the node that holds the given key:
    AVL_Node* iter = root;
    long long sum = 0;

    while (true) {
        if (iter->Salary < inSal) {
            sum += iter->SubGrades;
            iter = iter->right;
        }
        else if (iter->Salary > inSal) {
            iter = iter->left;
        }
        else {
            if (iter->ID > inID) {
                iter = iter->left;
            }
            else if (iter->ID < inID) {
                sum += iter->SubGrades;
                iter = iter->right;
            }
            else if (iter->ID == inID)
                break;
        }
    }

    return sum + iter->SubGrades;
}

Employee* AVL::getByIndex(int inIndex) {
    // Getting to the node that holds the given index:
    AVL_Node* iter = root;
    inIndex++;  // Incrementing the index as subnodes also include the nodes themselves

    while (iter != nullptr) {
        if (iter->SubNodes > inIndex)
            iter = iter->left;
        else if (iter->SubNodes < inIndex) {
            inIndex -= iter->SubNodes;
            iter = iter->right;
        }
        else if (iter->SubNodes == inIndex)
            break;
    }

    return iter->Emp;
}

Employee* AVL::getClosest(int inSal, bool Orient) {
    AVL_Node* iter = root;
    AVL_Node* current = root;
    bool FirstRun = true;

    while (iter != nullptr) {
        if (iter->Emp->getSal() > inSal) {
            if (iter->left == nullptr) break;

            iter = iter->left;
        }
        else if (iter->Emp->getSal() < inSal) {
            if (iter->right == nullptr) break;

            iter = iter->right;
        }
        else {
            if (FirstRun) {
                current = iter;
                FirstRun = false;
            }
            else {
                if (Orient && (current->ID < iter->ID))
                    current = iter;
                else if (!Orient && (current->ID > iter->ID))
                    current = iter;
            }

            if (Orient) {
                if (iter->right == nullptr) break;

                iter = iter->right;
            }
            else {
                if (iter->left == nullptr) break;

                iter = iter->left;
            }
        }
    }

    // Checking if no one is close :
    if (current == nullptr)
        return nullptr;

    // Return the data from the node :
    if (current->Salary == inSal)
        return current->Emp;
    else
        return iter->Emp;
}

StatusType AVL::AddNode(Employee* inData) {
    // Returns FAILURE on Existing items
    // Returns ALLOCATION_ERROR on malloc issues
    // Returns SUCCESS on successful add

    // Checking if the key already exits:
    if (nodeExists(inData->getSal(), inData->getID()) == true) return FAILURE;

    // Using a recursive function that starts in root to delete the node:
    try {
        root = AddNodeAux(root, root, inData->getSal(), inData->getID(), inData->getGrade(), inData);
        TotalGrades += inData->getGrade();
    }
    catch (StatusType result) {
        if (result != SUCCESS) return result;
    }
    return SUCCESS;
}

StatusType AVL::DeleteNode(int inSal, int inID, int inGrade) {
    // Returns FAILURE on Non-Existing items
    // Returns ALLOCATION_ERROR on malloc issues
    // Returns SUCCESS on successful add

    // Checking if the node exits:
    if (nodeExists(inSal, inID) == false) return FAILURE;

    // Using a recursive function that starts in root to delete the node:
    try {
        root = DeleteNodeAux(root, inSal, inID, inGrade);
        TotalGrades -= inGrade;
    }
    catch (StatusType result) {
        if (result != SUCCESS) return result;
    }
    return SUCCESS;
}

StatusType AVL::inOrder(Employee** dataArray) {
    int* counter = new int;
    if (counter == nullptr) return ALLOCATION_ERROR;

    *counter = 0;
    inOrderAux(dataArray, root, counter);
    delete counter;

    return SUCCESS;
}

StatusType AVL::replace(Employee** data, int size) {
    // Deleting whatever was in the tree before:
    destructorAux(root);
    root = nullptr;

    // Using a recursive function that adds each node with O(1) complexity:
    try {
        root = replaceAux(data, 0, size - 1, nullptr, 0);
        fixSubnodes(root, 0);
        fixSubgrades(root, 0);
        fixHeight(root);

        TotalGrades = 0;

        for (int i = 0; i < size; i++) {
            TotalGrades += data[i]->getGrade();
        }

        TreeSize = size;
    }
    catch (StatusType result) {
        if (result != SUCCESS) return result;
    }

    return SUCCESS;
}

Comparison AVL::compare(int Sal1, int ID1, int Sal2, int ID2) {
    if (Sal1 < Sal2) return KeyBigger;
    if (Sal1 > Sal2) return KeySmaller;
    if (Sal1 == Sal2) {
        if (ID1 < ID2) return BackupBigger;
        if (ID1 > ID2) return BackupSmaller;
        if (ID1 == ID2) return Equal;
    }

    return NotEqual;
}

void AVL::Print2D() { printBT("", root, false); }
