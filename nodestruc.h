//
// Created by Raul Gallardo on 3/21/24.
//

#ifndef INC_406PROJECT_NODESTRUC_H
#define INC_406PROJECT_NODESTRUC_H
struct Nodes{
    Token data;
    Nodes* leftChild;
    Nodes* rightSibling;
    Nodes(const Token& token): data(token),leftChild(nullptr),rightSibling(nullptr){}
};
Nodes* makenode(const Token &value);
void insertchild(Nodes* root , Nodes* child);
void insertsibiling(Nodes* root , Nodes* child);
void loadTokens(std::vector<Token> tokens);
void print(Nodes* root);

Nodes* makenode(const Token &value){
    return new Nodes(value);
}
void insertchild(Nodes* root, Nodes* newnode) {
    if (root == nullptr) {
        return;
    }

    if (root->leftChild == nullptr &&root->rightSibling == nullptr ) {

        root->leftChild = newnode;
    } else {

        insertchild(root->leftChild, newnode);
    }


    if (root->rightSibling != nullptr) {
        insertchild(root->rightSibling, newnode);
    }
}
void insertsibiling(Nodes* root, Nodes* newnode) {
    if (root == nullptr) {
        return;
    }

    if (root->rightSibling == nullptr && root->leftChild == nullptr) {
        root->rightSibling = newnode;
    } else {

        insertsibiling(root->rightSibling, newnode);
    }
    insertsibiling(root->leftChild, newnode);
}

void print(Nodes* root) {
    if (root == nullptr) {
        return;
    }


    std::cout << root->data.value << std::endl;


    print(root->rightSibling);



    if (root->leftChild != nullptr) {


        std::cout << "child -";
        print(root->leftChild);
    }
}






#endif //INC_406PROJECT_NODESTRUC_H
