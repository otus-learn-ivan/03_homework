#include "lib.h"
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <array>
#include <bitset>

template <class T, int size>
struct Trepository
{
    using Trepos  = std::array<T,size>;
    using Tmask_repos = std::bitset<size>;
    std::shared_ptr <Trepos> repos{new Trepos()};
    std::shared_ptr <Tmask_repos> mask_repos{new Tmask_repos()}; // 0 - блок свободен 1 - блок занят
    Trepository(){ }
    T* get(std::size_t n){
        if(n > sizeof(T)){
            throw "Error size blok get";
        }
        auto free_blok = get_number_free_blok();
        if(free_blok==-1){
             return nullptr;
        }
        auto  answer = &((*repos)[free_blok]);
       (*mask_repos)[free_blok] = true;
       return answer;
    }
    void free(T* blok,std::size_t n){
        if(n > sizeof(T)){
            throw "Error size blok free";
        }
       try{
            auto number = number_blok(blok);
            (*mask_repos)[number] = false;
       }
       catch(const char* error_message){
            cout << error_message <<endl;
            return;
       } 
    }
    int number_blok(T* blok_find){
        auto blok = find_if((*repos).begin(),(*repos).end(),[&blok_find](auto& cur_bloc){return &cur_bloc==blok_find;});
        if(blok==repos->end()){ throw "unknown block ";}
        auto number = (*repos).begin() - blok;
        return number;
    }
    int get_number_free_blok(){ // возврощает первый бит равный еденице
         for(int i = 0; i<size;i++){
            if(mask_repos->test(i)==0){
                return i;
            }
         }
         return -1; // хранилище переполнено все блоки заняты
    }

};


template <class T>
struct Tcustom_allocator {
    using value_type = T;
   
     using Tthis_repository = Trepository<T,10>;
     std::shared_ptr <Tthis_repository> repository {new Tthis_repository()};

    Tcustom_allocator () noexcept 
    { }

    template <class U> Tcustom_allocator(const Tcustom_allocator <U>& a) noexcept {
         repository = a.repository;
    }

     Tcustom_allocator select_on_container_copy_construction() const
    { 
        std::cout << "Tcustom_allocator::select_on_container_copy_construction()" << std::endl;
        return Tcustom_allocator(); 
    }

    T* allocate (std::size_t n)
    {
        return static_cast<T*>(repository->get(n));         
    }
    void deallocate (T* p, std::size_t n)
    {
        repository->free(p,n);
    }

    template< class U >
    struct rebind
    {
        typedef Tcustom_allocator<U> other;
    };

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type; //UB if std::false_type and a1 != a2;
};

template <class T, class U>
constexpr bool operator== (const Tcustom_allocator<T>& a1, const Tcustom_allocator<U>& a2) noexcept
{
    return a1.pool == a2.pool;
}

template <class T, class U>
constexpr bool operator!= (const Tcustom_allocator<T>& a1, const Tcustom_allocator<U>& a2) noexcept
{
    return a1.pool != a2.pool;
}

int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

template <typename _Tp, typename _Alloc = std::allocator<_Tp> > 
struct Tcontener {
    public:
      typedef _Tp	  value_type;
      typedef _Alloc  allocator_type;
      
      struct Node{
        std::shared_ptr<Node> next_node;
        value_type value;
      };
      
      using  Talloc =  typename allocator_type::rebind<Node>::other;

      Talloc allocator;

      struct Tdeleter
      {
        Talloc& allocator_;
        Tdeleter(Talloc& a ):allocator_(a){};
        void operator()(void * ptr){
            allocator_.deallocate(static_cast<Node*> (ptr),sizeof(Node));
        }
      };

      int count_nod;
      std::shared_ptr<Node> head_node;

      Tcontener():allocator(),count_nod(0),head_node(nullptr){}
      ~Tcontener(){
       while(head_node!=nullptr){
        if(head_node->next_node){
            {
            auto buf = head_node->next_node;
            head_node->next_node.reset();
            head_node.reset();
            head_node  =  buf;
            }
        }
        else{
            head_node.reset();
        }
       }
       allocator.~Talloc();
      }

      value_type  operator[](int number_node){
        auto start = head_node;
        auto lok_count = count_nod - number_node -1;
        if(lok_count < 0 ) throw "number_node more count_nod ";\
        while(lok_count--) start = start->next_node; //мотаем на начало
        return start->value;
      }

     void add_node(value_type value){
        head_node = std::shared_ptr<Node>(new(allocator.allocate(sizeof(Node))) Node{head_node,value},Tdeleter(allocator));
        count_nod++;
     }
      
};

int main (int, char **) {
    std::cout << "Version: " << version() << std::endl;
    {
        std::map<int, int> map_def_allok;
        for(int i=0;i<10;i++){
            map_def_allok.try_emplace(i,factorial(i));
        }
        for(const auto& [key,value] : map_def_allok){
            cout << "map_def_allok["<< key << "] " << value << "\n";
        }
    }
    {
        std::map<int, int, std::less<int>, Tcustom_allocator<std::pair<const int, int> > > map_custom_allok;
        for(int i=0;i<10;i++){
            map_custom_allok.try_emplace(i,factorial(i));
        }
        for(const auto& [key,value] : map_custom_allok){
            cout << "map_custom_allok["<< key << "] " << value << "\n";
        }
    }
    {
        Tcontener<int> contener_def_allok{};
        for(int i=0;i<10;i++){
            contener_def_allok.add_node(i);
        }
        try
        {
            for(int i=0;i<10;i++){
            cout << "contener_def_allok["<< i << "] " << contener_def_allok[i] << endl;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    {
        Tcontener<int,Tcustom_allocator<int> > contener_custom_allok{};
        for(int i=0;i<10;i++){
            contener_custom_allok.add_node(i);
        }
        try
        {
            for(int i=0;i<10;i++){
            cout << "contener_custom_allok["<< i << "] " << contener_custom_allok[i] << endl;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}
