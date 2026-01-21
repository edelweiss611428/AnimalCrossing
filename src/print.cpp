#include <Rcpp.h>
using namespace Rcpp;

// =================================================
// Animal base class
// =================================================

//' @name Animal
//' @title Base Animal class
class Animal {
 protected:
   std::string type_;
   std::string name_;
   int age_;

 public:
   Animal(std::string type, std::string name, int age)
     : type_(type), name_(name), age_(age) {}

   virtual ~Animal() {}

   virtual std::string type() const { return type_; }
   virtual std::string name() const { return name_; }
   virtual int age() const { return age_; }
 };

RCPP_EXPOSED_CLASS(Animal)

// =================================================
// Duck
// =================================================

//' @name Duck
//' @title Duck class
//' @export
class Duck : public Animal {
 public:
   Duck(std::string name, int age)
     : Animal("duck", name, age) {}
 };

RCPP_EXPOSED_CLASS(Duck)

// =================================================
// Dog
// =================================================

//' @name Dog
//' @title Dog class
//' @export
class Dog : public Animal {
 public:
   Dog(std::string name, int age)
     : Animal("dog", name, age) {}
 };

RCPP_EXPOSED_CLASS(Dog)

// =================================================
// RAnimal
// =================================================

//' @name RAnimal
//' @title RAnimal generic class
//' @export
class RAnimal : public Animal {
 private:
   Function type_fun_;
   Function name_fun_;
   Function age_fun_;

 public:
   RAnimal(Function type_fun,
           Function name_fun,
           Function age_fun)
     : Animal("", "", 0),
       type_fun_(type_fun),
       name_fun_(name_fun),
       age_fun_(age_fun) {}

   std::string type() const override {
     return as<std::string>(type_fun_());
   }

   std::string name() const override {
     return as<std::string>(name_fun_());
   }

   int age() const override {
     return as<int>(age_fun_());
   }
 };

RCPP_EXPOSED_CLASS(RAnimal)

// =================================================
// print_animal
// =================================================

//' Print animal
//' @name print_animal
//' @param animalSEXP Duck, Dog, or RAnimal
//' @export
 // [[Rcpp::export]]
void print_animal(SEXP animalSEXP) {
   const Animal& animal = Rcpp::as<const Animal&>(animalSEXP);

   Rcpp::Rcout
   << animal.name()
   << " is a "
   << animal.type()
   << " and they are "
   << animal.age()
   << " year old"
   << std::endl;
}

// =================================================
// Module
// =================================================
RCPP_MODULE(animal_module) {

  class_<Animal>("Animal"); // conversion only

  class_<Duck>("Duck")
    .constructor<std::string, int>();

  class_<Dog>("Dog")
    .constructor<std::string, int>();

  class_<RAnimal>("RAnimal")
    .constructor<Function, Function, Function>();
}
