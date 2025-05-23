#include <iostream>
#include <algorithm>
#include<limits>
#include <vector>
#include <queue>
#include <limits>
#include <chrono>
#include <unordered_map>
#include <math.h>
#include <chrono>

using namespace std;


const unsigned int INF = -1;  
class Node;
class NodeHashTable;

bool isValid2DField(int x, int y, int size) {
    return (x >= 0 && x < size && y >= 0 && y < size);
}

vector<int> findNeighbours(int field, int size){
    int x = field / size;
    int y = field % size;
    vector<int> neighbours;
    neighbours.reserve(6);

    if (field < 0 || field >= size*size) {
        return neighbours; 
    }

    vector<pair<int, int>> NeighboursTwoD = {
        {x-1, y-1}, // up
        {x-1, y},  // up right
        {x, y-1}, // up left
        {x, y+1}, // down right
        {x+1, y}, // down left
        {x+1, y+1}, // down
        
    };

    for (const auto& neighbour : NeighboursTwoD) {
        int newX = neighbour.first;
        int newY = neighbour.second;
        if(isValid2DField(newX, newY, size)) {
            int newField = newX * size + newY;
            neighbours.push_back(newField);
        }
    }

    
    return neighbours;
}


bool isStartingField(int field, int size, bool isMaxPlayer) {
    if (field < 0 || field >= size*size || size <= 0) {
        return false; // Invalid field
    }

    if (isMaxPlayer) {
        return field >= 0 && field < size;
    }else{
        return field % size == 0;
    }
}

bool isEndField(int field, int size, bool isMaxPlayer) {
    if (isMaxPlayer) {
        return field >= size*(size-1) && field < size*size;
    }else{
        return field % size == size - 1;
    }
}

bool winnerPathBfs(int size, bool isMaxPlayer,string board) {
    queue<int> queue;
    vector<bool> visited(size*size, false);

    //get start fields
    vector<int> startingFields;
    startingFields.reserve(size);
    if (isMaxPlayer){
        for (int i = 0; i < size; i++) {
            if (board[i] == '1') {
                startingFields.push_back(i);
            }
        }
    }else{
        for (int i = 0; i < size*size; i+=size){
            if (board[i] == '2') {
                startingFields.push_back(i);
            }
        }
    }

    while (!startingFields.empty()) {
        queue.push(startingFields.back());
        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();
            visited[current] = true;

            if (isEndField(current, size, isMaxPlayer)) {
                return true;
            }

            for(int neighbour : findNeighbours(current, size)) {
                if(!visited[neighbour] && board[neighbour] == (isMaxPlayer ? '1' : '2')) {
                    queue.push(neighbour);
                    visited[neighbour] = true;
                }
            }
        
        }
        startingFields.erase(
            remove_if(startingFields.begin(), startingFields.end(), [&](int endField) {
                return visited[endField];
            }),
            startingFields.end()
        );
    }
    return false;
}

class NodeHashTable {
public:
    unordered_map<string, Node*> table;
     

    static NodeHashTable& getInstance() {
        static NodeHashTable instance;
        return instance;
    }
    void addNode(Node* node);
    Node* getNode(const string& board);
    bool contains(const string& board);
    void removeNode(const string& board);
    int byteSize();


private:
    NodeHashTable() {table.reserve(100000000);}
    ~NodeHashTable() {}
    NodeHashTable(const NodeHashTable&) = delete;
    NodeHashTable& operator=(const NodeHashTable&) = delete;
};

class GameState {
public:
    string board;
    bool isMaxPlayer;
    int size;
        
    GameState() : board(""), isMaxPlayer(false), size(0) {}

    GameState(string board, bool isMaxPlayer) {
        this->board = board;
        this->isMaxPlayer = isMaxPlayer;
        this->size = sqrt(board.size());
    }
    
    int getLowestField(int column_param) {

        int column = column_param - size;
        int x = size-1, y = size-1;
        if(column <= 0) y += column;
        else x -= column;

        int lowest = x*size + y;

        while(board[lowest] != '0') {
            x--;
            y--;
            if(x < 0 || y < 0) throw out_of_range("No more space in column");
            lowest = x*size + y;
        }
        return lowest;
    }

    string getNextMove(int column, string board_param) {
        if(column < 1 || column > size*2-1) throw out_of_range("Invalid column index");

        int lowest = getLowestField(column);
        string new_board = board_param;
        new_board[lowest] = isMaxPlayer ? '1' : '2';
        return new_board;
    }

    int byteSize() {
        return size*size+1 + sizeof(isMaxPlayer) + sizeof(size);
    }
};

class Node {
public:
    GameState state;
    int depth = 0;
    vector<Node*> children;
    vector<Node*> parents;
    vector<int> possible_moves;
    int pn = 1;
    int dn = 1;
    int value = 0;



    Node(GameState state, vector<int> possible_moves,int depth = 0) {
        this->state = state;
        this->value = 0;
        this->possible_moves = possible_moves;
        this->depth = depth;
    }

    void updateProofNumbers(Node* node, bool recursive = false) {
        if(node->children.empty()){
            if(node->value !=0){
                if(node->value == 1){
                    node->pn = 0;
                    node->dn = INF;
                }else{
                    node->dn = 0;
                    node->pn = INF;
                }
            }else{
                if (node->state.isMaxPlayer){
                    node->pn = 1;
                    node->dn = node->possible_moves.size();
                }else{
                    node->dn = 1;
                    node->pn = node->possible_moves.size();
                }
            }
        }else{
            vector<unsigned int> childProofs;
            childProofs.reserve(node->children.size());
            vector<unsigned int> childDisProofs;
            childDisProofs.reserve(node->children.size());
            for(Node* child : node->children){
                if(recursive){
                    child->updateProofNumbers(child, recursive);
                }
                childProofs.push_back(child->pn);
                childDisProofs.push_back(child->dn);
            }
            if (node->state.isMaxPlayer){
                // find the minimum proof number that is not INF
                node->pn = *min_element(childProofs.begin(), childProofs.end());
                node->dn = 0;
                for(int i = 0; i < childDisProofs.size(); i++){
                    if (childDisProofs[i] == INF){
                        node->dn = INF;
                        break;
                    }
                    node->dn += childDisProofs[i];
                }
            }else{
                node->dn = *min_element(childDisProofs.begin(), childDisProofs.end());
                node->pn = 0;
                for(int i = 0; i < childProofs.size(); i++){
                    if (childProofs[i] == INF){
                        node->pn = INF;
                        break;
                    }
                    node->pn += childProofs[i];
                }
            }
        }   
    }

    void deleteSubTree(){
        while (!children.empty()) {
            Node* child = children.back();
            children.pop_back();
            child->deleteSubTree();
        }
        children.clear();
        for (Node* parent : parents) {
            auto it = find(parent->children.begin(), parent->children.end(), this);
            if (it != parent->children.end()) {
                parent->children.erase(it);


            }
        }
        NodeHashTable& hashTable = NodeHashTable::getInstance();
        hashTable.removeNode(state.board);
        delete this;
    }

    void updateProofParents(int min_depth = 0){
        for(Node* parent : parents) {
            if(parent->depth < min_depth) continue;
            parent->updateProofNumbers(parent, false);
            parent->updateProofParents();
        }
    }

    int pn2Search(Node* root, int depth1, int depth2){
        root->getChildren(depth1);
        updateProofNumbers(root, true);
        Node* mostProvingNode;
        int iterations = 0;
        while (root->pn!=0 && root->dn!=0){
            mostProvingNode = root->getMostProvingNode();
            mostProvingNode->getChildren(mostProvingNode->depth + depth2);
            mostProvingNode->updateProofNumbers(mostProvingNode, true);
            mostProvingNode->updateProofParents();
            
            
            if(mostProvingNode->pn == 0 || mostProvingNode->dn == 0){
                mostProvingNode->deleteSubTree();
            }

            //Uncomment this to delete the least proving node
            //Node* leastProvingNode = root->getLeastProvingNode();
            //leastProvingNode->deleteSubTree();
            iterations++;
            if (iterations % 1000 == 0) {
                cout << "Iterations: " << iterations << ", Nodes: " << NodeHashTable::getInstance().table.size() << endl;
            }

        }

        mostProvingNode= root->getMostProvingNode(true);
        cout << "\t" << flush;
        
        mostProvingNode->getChildren(mostProvingNode->depth + depth2);
        mostProvingNode->updateProofNumbers(mostProvingNode, true);
        mostProvingNode->getMostProvingNode(true);
        
        if (root->pn == 0){
            cout << "First player wins" << endl;
            return 1;
        }else if (root->dn == 0){
            cout << "Second player wins" << endl;
            return -1;

        }
        cout << "Algorithm failed" << endl;
        return 0;
    }
    Node* getMostProvingNode(bool verbose=false){
        if(!children.empty()){
            if (this->state.isMaxPlayer){
                for (int i = 0; i < children.size(); i++) {
                    if (children[i]->pn == this->pn) {
                        if (verbose) {
                            for (int move:this->possible_moves)
                            {
                                string board = this->state.getNextMove(move, this->state.board);
                                if(board == children[i]->state.board){
                                    cout << move << flush;
                                    break;
                                }
                            }                            
                        }
                        return children[i]->getMostProvingNode(verbose);
                    }
                }
            }else{
                for (int i = 0; i < children.size(); i++) {
                    if (children[i]->dn == this->dn) {
                        if (verbose) {
                            for (int move:this->possible_moves)
                            {
                                string board = this->state.getNextMove(move, this->state.board);
                                if(board == children[i]->state.board){
                                    cout << move << flush;
                                    break;
                                }
                            }                         
                        }
                        return children[i]->getMostProvingNode(verbose);
                    }
                }
            }
        }
        return this;
    }

    Node* getLeastProvingNode(){
        if(!children.empty()){
            if (this->state.isMaxPlayer){
                for (int i = 0; i < children.size(); i++) {
                    return (*max_element(children.begin(), children.end(), [](Node* a, Node* b) {
                            return a->pn < b->pn;
                        }))->getLeastProvingNode();
                    } 
            }else{
                for (int i = 0; i < children.size(); i++) {
                    return (*max_element(children.begin(), children.end(), [](Node* a, Node* b) {
                            return a->dn < b->dn;
                        }))->getLeastProvingNode();
                }
            }
        }
        return this;
    }

    void getChildren(int max_depth=4) {
        for(int i = 0; i < this->possible_moves.size(); ) {
            int move = this->possible_moves[i];
            try {
                string next_board = state.getNextMove(move, this->state.board);
                // Check if the new board is already in the hash table
                NodeHashTable& hashTable = NodeHashTable::getInstance();
                Node* child = nullptr;
                if(hashTable.contains(next_board)) {
                    child = hashTable.getNode(next_board);
                    child->parents.push_back(this);
                    children.push_back(child);

                    i++;
                }else{
                    // If not, create a new node and add it to the hash table            
                    GameState new_state(next_board, !state.isMaxPlayer);
                    try
                    {
                        new_state.getNextMove(move, new_state.board);
                        child = new Node(new_state, possible_moves, this->depth + 1);
                    }
                    catch(const out_of_range& e)
                    {
                        //possible moves filtered out
                        
                        vector <int > next_possible_moves;
                        next_possible_moves.reserve(possible_moves.size()-1);
                        next_possible_moves.insert(next_possible_moves.end(), possible_moves.begin(), possible_moves.begin() + i);
                        next_possible_moves.insert(next_possible_moves.end(), possible_moves.begin() + i + 1, possible_moves.end());
                        
                        child = new Node(new_state, next_possible_moves, this->depth + 1);
                    }
                    
                    child->parents.push_back(this);
                    children.push_back(child);
                    if(winnerPathBfs(state.size, state.isMaxPlayer, child->state.board)) {
                        child->value = state.isMaxPlayer ? 1 : -1;
                    }
                    child->depth = this->depth + 1;
                    hashTable.addNode(child);
                    if(child->depth < max_depth && child->value == 0) {
                        child->getChildren(max_depth);
                    }
                    i++;
                }
            }catch(const out_of_range& e) {
                possible_moves.erase(possible_moves.begin() + i);
                continue;
            } catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
                i++;
            }
        }
    };

    int byteSize(bool includeChildren = true) {
        int size = sizeof(depth) + sizeof(pn) + sizeof(dn) + sizeof(value);
        size += state.byteSize();
        if (includeChildren) {   
            for(Node* child : children) {
                size += child->byteSize();
            }
        }
        size += sizeof(children);
        size += possible_moves.size() * sizeof(int);
        return size;
    }
};

void NodeHashTable::addNode(Node* node) {
    table[node->state.board] = node;
}

Node* NodeHashTable::getNode(const string& board) {
    if (table.find(board) != table.end()) {
        return table[board];
    }
    return nullptr;
}

bool NodeHashTable::contains(const string& board) {
    return table.find(board) != table.end();
}

int NodeHashTable::byteSize() {
        long long size = 0;
        for (const auto& pair : table) {
            size += pair.first.size() + sizeof(pair.second);
            size += pair.second->byteSize(false);
        }
        return size;
    }

void NodeHashTable::removeNode(const string& board) {
    auto it = table.find(board);
    if (it != table.end()) {
        Node* node = it->second;
        table.erase(it);
    }
}

int main() {
    string a(9,'0');
    GameState startState(a, true);
    
    vector<int> possible_moves;
    possible_moves.reserve(startState.size*2);
    for(int i = 1; i < startState.size*2; i++){
        possible_moves.push_back(i);
    }
    
    Node root(startState, possible_moves, 0);

    auto start = chrono::high_resolution_clock::now();

   root.pn2Search(&root, 8,26);
    


    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "Time taken: " << duration.count() << " microseconds" << endl;
    NodeHashTable& hashTable = NodeHashTable::getInstance();
    cout << "Hash table size: " << hashTable.table.size() << endl;
    cout << "Size of int: " << sizeof(int) << " bytes\n"; 

    
    cout << "Size of root: " << root.byteSize(false) << endl;
    cout << "Size of hash table: " << sizeof(hashTable.table) << endl;

    cout << "Hashtable size \t Root size \t Time taken \t Hash table size(bytes)" << endl;
    cout << hashTable.table.size() << "\t" << root.byteSize(false) << "\t" << duration.count() << "\t" << hashTable.byteSize() << endl;
    return 0;
}
