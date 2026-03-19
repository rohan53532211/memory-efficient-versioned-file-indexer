#include <bits/stdc++.h>
using namespace std;

//  filereader: This class reads a files using a fixed-size buffer.
class filereader{
private:
    ifstream file;
    vector<char> buffer;
    size_t buffersize;
    size_t index;
    size_t length;
public:
    filereader(const string& filepath, size_t bufsizekb){
        buffersize = bufsizekb * 1024;
        index = 0;
        length = 0;
        file.open(filepath, ios::binary);
        if(!file){
            throw runtime_error("File not found:");
        }
        buffer.resize(buffersize);
    }
    bool getNextChar(char& c) {
        if(index >= length){
            if(!file.good()){
                return false;
            }
            file.read(buffer.data(), buffersize);
            length= file.gcount();
            index= 0;
            if (length== 0) {
                return false;
            }
        }
        c = buffer[index++];
        return true;
    }
};

// Tokenizer: converts the file characters into words (lower case).
class tokenizer {
private:
    filereader &reader;
public:
    tokenizer(filereader &r) : reader(r){}

    string nexttoken() {
        string token;
        char c;
        while(true){
            if(!reader.getNextChar(c)){
                break;
            }
            if(isalnum(static_cast<unsigned char>(c))){
                token += tolower((unsigned char)c);
                while(true){
                    if(!reader.getNextChar(c) || !isalnum(static_cast<unsigned char>(c))){
                        break;
                    }
                    token += tolower((unsigned char)c);
                }
                break;
            }
        }
        return token;
    }
};

// Template to get top-K  frequent words using word-frequency map.
template<typename maptype>
vector<pair<typename maptype::key_type, typename maptype::mapped_type>>
gettopK(const maptype &freqmap, int k) {
    vector<pair<typename maptype::key_type, typename maptype::mapped_type>> items;
    for(auto it = freqmap.begin(); it != freqmap.end(); ++it){
        items.push_back(make_pair(it->first, it->second));
    }
    sort(items.begin(), items.end(), [](const auto& a, auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    if(k<(int)items.size()){
        items.resize(k);
    }
    return items;
}

// Versioned index: stores word frequencies for each version of the file.
class versionindex{
private:
    unordered_map<string, unordered_map<string,int>> versions;
public:
    void add_word(const string &version, const string &word){
        versions[version][word]++;
    }
    int get_word_count(const string &version, const string &word){
        string w = word;
        transform(w.begin(), w.end(), w.begin(), ::tolower);
        auto it = versions.find(version);
        if(it != versions.end()){
            auto &fmap = it->second;
            auto fit = fmap.find(w);
            if(fit != fmap.end()){
                return fit->second;
            }
        }
        return 0;
    }
    int get_count(const string &version, const char *word){
        return get_word_count(version, string(word));
    }
    int get_diff(const string &version1, const string &version2, const string &word){
        int c1 = get_word_count(version1, word);
        int c2 = get_word_count(version2, word);
        return c2 - c1;
    }
    vector<pair<string,int>> topwords(const string &version, int k){
        auto it = versions.find(version);
        if (it == versions.end()) {
            return {};
        }
        return gettopK(it->second, k);
    }
};

// Index Builder: builds a Versioned index from input files.
class indexbuilder {
public:
    void addversion(versionindex &index, const string &file_path, const string &version, size_t buffer_size_KB){
        filereader reader(file_path, buffer_size_KB);
        tokenizer tokenize(reader);
        string token;
        while(true){
            token = tokenize.nexttoken();
            if(token.empty()){
                break;
            }
            index.add_word(version, token);
        }
    }
};

// Query: abstract base class for queries.
class query {
protected:
    versionindex &index;
public:
    query(versionindex &idx) : index(idx){}
    virtual ~query(){}
    virtual void execute() = 0;
};

// Word count query: perform word count for a given version.
class word_count_query:public query{
private:
    string version;
    string word;
public:
    word_count_query(versionindex &idx, const string &version, const string &word)
        : query(idx), version(version), word(word){}
    void execute() override{
        string w = word;
        transform(w.begin(), w.end(), w.begin(), ::tolower);
        int count = index.get_word_count(version, w);
        cout << "Count of \"" << word << "\" in " << version << ": " << count << endl;
    }
};

// Different query: compare frequency of a word between two versions.
class diff_query : public query{
private:
    string version1;
    string version2;
    string word;
public:
    diff_query(versionindex &idx, const string &version1, const string &version2, const string &word)
        : query(idx), version1(version1), version2(version2), word(word){}
    void execute() override{
        string w = word;
        transform(w.begin(), w.end(), w.begin(), ::tolower);
        int diff = index.get_diff(version1, version2, w);
        cout << "Frequency difference of \"" << w << "\" between " << version1 << " and " << version2 << ": " << diff <<endl;
    }
};

// TopK query: list of top K words by frequency for a versions.
class topk_query : public query{
private:
    string version;
    int k;
public:
    topk_query(versionindex &idx, const string &version, int k)
        : query(idx), version(version), k(k){}
    void execute() override{
        auto topWords = index.topwords(version, k);
        cout << "Top " << k << " words in " << version << ":\n";
        for (const auto& p : topWords) {
            cout << p.first << ": " << p.second << endl;
        }
    }
};

// Main function

int main(int argc, char* argv[]){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    try{
        if(argc <2){
            throw runtime_error("Insufficient arguments");
        }
        string file1, version1, file2, version2, query_type, word;
        int buffer_size_KB = 256;
        int topK = 0;
        for(int i=1;i<argc;i++){
            string arg = argv[i];
            if(arg == "--file" && i+1<argc){
                file1 = argv[++i];
            }else if(arg == "--version" && i+1<argc){
                version1 = argv[++i];
            }else if(arg == "--file1" && i+1<argc){
                file1 = argv[++i];
            }else if(arg == "--version1" && i+1<argc){
                version1 = argv[++i];
            }else if(arg == "--file2" && i+1<argc){
                file2 = argv[++i];
            }else if(arg == "--version2" && i+1<argc){
                version2 = argv[++i];
            }else if(arg == "--query" && i+1<argc){
                query_type = argv[++i];
            }else if(arg == "--word" && i+1<argc){
                word = argv[++i];
            }else if(arg == "--top" && i+1<argc){
                topK = stoi(argv[++i]);
            }else if(arg == "--buffer" && i+1<argc){
                buffer_size_KB = stoi(argv[++i]);
            }
        }
        if(buffer_size_KB<256 || buffer_size_KB>1024){
            throw runtime_error("Buffer size must be between 256 and 1024 KB");
        }
        if(query_type.empty()){
            throw runtime_error("Query type not specified");
        }

        versionindex index;
        indexbuilder builder;
        auto start = chrono::high_resolution_clock::now();

        if(query_type == "word"){
            if(file1.empty() || version1.empty() || word.empty()){
                throw runtime_error("Missing arguments for word count query");
            }
            builder.addversion(index, file1, version1, buffer_size_KB);
            word_count_query query(index, version1, word);
            cout<<"Version: "<<version1<<"\n";
            query.execute();
        }else if(query_type == "diff"){
            if(file1.empty() || version1.empty() || file2.empty() || version2.empty() || word.empty()){
                throw runtime_error("Missing arguments for diff query");
            }
            builder.addversion(index, file1, version1, buffer_size_KB);
            builder.addversion(index, file2, version2, buffer_size_KB);
            diff_query q(index, version1, version2, word);
            cout<<"Versions: "<<version1<<", "<<version2<<"\n";
            q.execute();
        }else if(query_type == "top"){
            if(file1.empty() || version1.empty() || topK <= 0){
                throw runtime_error("Missing arguments for top-K query");
            }
            builder.addversion(index, file1, version1, buffer_size_KB);
            topk_query query(index, version1, topK);
            cout<<"Version: "<<version1<<"\n";
            query.execute();
        }else{
            throw runtime_error("Unknown query type");
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout<<"Buffer size: "<<buffer_size_KB<<" KB\n";
        cout<<"Total execution time: "<<duration<<" ms\n";
    }
    catch(const exception &e){
        cerr<<"Error: "<< e.what()<<"\n";
        return 1;
    }
    return 0;
}