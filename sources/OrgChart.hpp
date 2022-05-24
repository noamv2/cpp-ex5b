#include <vector>
#include <iterator>
#include <iostream>
#include <stack>
#include <queue>



namespace ariel {
template <class T> class Iterator;
enum traversalOrder {PRE, LEVEL, REVRSE};
template <typename T = std::string> class Node{

private:
    
    T data;
    Node * parent;
    std::vector<Node<T>*> subs;

    
public:
    Node(T d):parent{nullptr}, data{d}{}
    Node(T d, Node<T> * par):parent{par}, data{d}{}
    
    //getters/setters
    void setData(T d){data = d;}
    void addChild(Node<T>* c){subs.push_back(c);}
    
    
    Node<T>* getParent(){return parent;}
    T & getData(){return data;}
    std::vector<Node<T>*> & getSubs(){return subs;}
    //methods
    Node<T> * find(T toFind){
        
        if(toFind == data){
            return this;
        }

        for(Node<T>* n: subs){// Search in the subs
            Node<T>*temp = n->find(toFind);
            if(temp){return temp;}
        }

        return nullptr;
    }
};

template <class T = std::string> class OrgChart{
private:
    
    Node<T>* root;

public:

    OrgChart():root{nullptr}{}
    OrgChart(OrgChart & o){}
    OrgChart(OrgChart && o) noexcept{}
    OrgChart & operator=(OrgChart og){return *this;}
    OrgChart & operator=(OrgChart &&og) noexcept{return *this;}
    Node<T>* getRoot(){return root;}
    //methods
    OrgChart & add_root(T ceo){

        if(!root){// tree is empty
            root = new Node(ceo);
        }
        else{
            root->setData(ceo);
        }
        return *this;
    }

    ~OrgChart(){
        freeChart(root);
    }

    void freeChart(Node<T>* root){
        //Traverse the char in post order andd free all the nodes
        if(root ==nullptr){
            return;
        }
        for(size_t i = 0; i <root->getSubs().size(); i++ ){
            freeChart(root->getSubs().at(i));
        }
        delete root;
    }

    OrgChart & add_sub(T manager, T sub){
        //find the manager
        if(!root){
            throw std::invalid_argument("Tree is empty");
        }
        Node<T> * mang = root->find(manager);
        if(!mang){
            throw std::invalid_argument("Manager does not exists");
        }
        mang->addChild(new Node(sub, mang));

        return *this;
    }

    OrgChart<T> & add_sub(T manager, OrgChart<T> &og){
        //find the manager
        if(!root){
            throw std::invalid_argument("Tree is empty");
        }
        Node<T> * mang = root->find(manager);
        if(!mang){
            throw std::invalid_argument("Manager does not exists");
        }
        mang->addChild(og.getRoot());
        return *this;
    }
    
    static void print(Node<T>* root , std::ostream& os){
        if(!root){return;}
        os<<root->getData()<<"~";

        for(auto n: root->getSubs()){
            print(n, os);
        }
    }

    friend std::ostream& operator<<( std::ostream& os, OrgChart & og){
        print(og.getRoot(), os);
        // print(og.getRoot(), os, "", true);
        return os;
    }

    // static void print(Node<T>* root, std::ostream& os, std::string prefix, bool isLast){

    //     os<<prefix<<"+- "<<root->getData()<<std::endl;
    //     prefix += (isLast) ? "   " : "|  ";

    //     for(size_t i = 0; i < root->getSubs().size(); i++){
    //         print(root->getSubs().at(i), os, prefix, i + 1 == root->getSubs().size());
    //     } 

    // }


    //iterator generators
    Iterator<T> begin_preorder(){
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }        
        return Iterator{root, PRE};
    }
    Iterator<T> end_preorder(){
        // find the rightmost node and use it to create iterator
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        Node<T>* r = root;
        while(r->getSubs().size()){
            r = r->getSubs().at(r->getSubs().size() - 1); // r become the rightest child
        }
        return Iterator<T>{PRE};
    }

    Iterator<T> begin_level_order(){
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        return Iterator{root, LEVEL};
    }
    Iterator<T> end_level_order(){
        // find the rightmost node and use it to create iterator
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        return Iterator<T>{LEVEL};
    }
    Iterator<T> begin_reverse_order(){
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        return Iterator{root, REVRSE};
    }
    Iterator<T> end_reverse_order(){
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        // find the rightmost node and use it to create iterator
        return Iterator<T>{REVRSE};
    }
    Iterator<T> reverse_order(){
        if(root == nullptr){
            throw std::invalid_argument("bla");
        }
        return end_reverse_order();
    }
    Iterator<T> begin(){
        return begin_level_order();
    }
    Iterator<T> end(){
        return end_level_order();
    }

    
};

template <class T> class Iterator{

private:
    Node<T>* current;
    std::stack<Node<T>*> stk;
    std::queue<Node<T>*> que;
    int type;
public:
    Iterator(traversalOrder order):current{nullptr}, type{order}{}
    Iterator(Node<T>* ptr, traversalOrder order):current{ptr}, type{order}{
        stk.push(nullptr);
        if(type == REVRSE){

                if(!ptr){return;}

                    que.push(ptr);

                    while(!que.empty()){
                        // push the front of the queue to the vector
                        ptr = que.front();
                        que.pop();
                        stk.push(ptr);

                        //enque the children
                        for(int i = ptr->getSubs().size() - 1; i >= 0; i--){
                            que.push(ptr->getSubs().at(static_cast<size_t>(i)));
                        }
                    }
                    current = stk.top();
                    stk.pop();
        }
        else{
            que.push(ptr);
        }
    }

    //operators
    Iterator & operator++(){
        if(current){
            switch (type)
            {
            case PRE:
                
                next_pre();
                
                break;
            case LEVEL:
                next_level();
                break;
            case REVRSE:
                next_reverse();
                break;
            default:
                break;
            }
        }
        return *this;
    }

    const T & operator*(){
        if(!current){
            throw std::invalid_argument("cant derefrence nullptr");
        }
        return current->getData();
    }

    T* operator->() const {
        return &current->getData();
    }

    bool operator!=(Iterator<T> it){
        if(!this->current && ! it.current){ //in case both null
            return false;
        }
        if(!this->current || ! it.current){ //in one is null
            return true;
        }
        return this->current->getData() != it.current->getData();
    }
    


    void next_pre(){
        std::vector<Node<T>*> vec = current->getSubs();
        if(!vec.empty()){ 
            for(size_t i = vec.size() -1; ; --i){ //insert the subs in pre order logic
                stk.push(vec.at(i));
                if(i == 0){ break;}
            }
        }
        current = stk.top();
        stk.pop();
    }

    void next_level(){
        std::vector<Node<T>*> vec = current->getSubs();
        if(!vec.empty()){ 
            for(size_t i = 0 ;i < vec.size() ; ++i){ //insert the subs in pre order logic
                que.push(vec.at(i));   
            }
        }
        que.pop();
        if(que.empty()){
            current = nullptr;
        }
        else{
            current = que.front();       
        }
    }

    void next_reverse(){
        current = stk.top();
        stk.pop();
    }


};

}