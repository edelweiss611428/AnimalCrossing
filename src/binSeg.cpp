#include <Rcpp.h>
#include <vector>
#include <limits>

using namespace Rcpp;

// ============================================================
// Base cost class
// ============================================================

//' @name CostBase
//' @title Base cost class
class CostBase {
 public:
   virtual ~CostBase() {}
   virtual double eval(int start, int end) const = 0;
};

RCPP_EXPOSED_CLASS(CostBase)

// ============================================================
// L2 cost (pure C++); FAST version (O(1) per eval)
// ============================================================

//' @name CostL2
//' @title L2 cost (fast, prefix sums)
//' @export
class CostL2 : public CostBase {
 private:
   std::vector<double> csum_;   // cumulative sum
   std::vector<double> csum2_;  // cumulative sum of squares

 public:
   CostL2(NumericVector x) {
     int n = x.size();
     csum_.resize(n + 1);
     csum2_.resize(n + 1);

     csum_[0]  = 0.0;
     csum2_[0] = 0.0;

     for (int i = 0; i < n; ++i) {
       csum_[i + 1]  = csum_[i]  + x[i];
       csum2_[i + 1] = csum2_[i] + x[i] * x[i];
     }
   }

   double eval(int start, int end) const override {
     int n = end - start;
     if (n <= 0) return 0.0;

     double sum  = csum_[end]  - csum_[start];
     double sum2 = csum2_[end] - csum2_[start];

     // SSE = sum (x - mean)^2 = sum(x^2) - sum(x)^2 / n
     return sum2 - (sum * sum) / n;
   }
};

RCPP_EXPOSED_CLASS(CostL2)


// ============================================================
// R-defined cost
// ============================================================

//' @name RCostClass
//' @title R-defined cost
//' @export
class RCostClass : public CostBase {
 private:
   Function cost_fun_;

 public:
   RCostClass(Function cost_fun)
     : cost_fun_(cost_fun) {}

   double eval(int start, int end) const override {
     return as<double>(cost_fun_(start, end));
   }
};

RCPP_EXPOSED_CLASS(RCostClass)

// ============================================================
// Binary segmentation (single split)
// ============================================================

//' @name BinarySegmentation
//' @title Binary segmentation (single split)
//' @export
class BinarySegmentation {
 private:
   const CostBase& cost_;

 public:
   // IMPORTANT: take SEXP, convert manually
   BinarySegmentation(SEXP costSEXP)
     : cost_(Rcpp::as<const CostBase&>(costSEXP)) {}

   int predict(int start, int end) const {

     double totalErr = cost_.eval(start, end);
     double minErr   = std::numeric_limits<double>::infinity();
     int bestCp      = -1;

     for (int cp = start + 1; cp <= end - 1; ++cp) {
       double err =
         cost_.eval(start, cp) +
         cost_.eval(cp, end);

       if (err < minErr) {
         minErr  = err;
         bestCp = cp;
       }
     }

     Rcout << "Change point in ["
           << start << ", " << end
           << ") at " << bestCp
           << " | gain = " << (totalErr - minErr)
           << "\n";

     return bestCp;
   }
};

RCPP_EXPOSED_CLASS(BinarySegmentation)

  // ============================================================
  // Module
  // ============================================================
RCPP_MODULE(binseg_module) {

    // conversion-only base
    class_<CostBase>("CostBase");

    class_<CostL2>("CostL2")
      .derives<CostBase>("CostBase")
      .constructor<NumericVector>()
      .method("eval", &CostL2::eval);

    class_<RCostClass>("RCostClass")
      .derives<CostBase>("CostBase")
      .constructor<Function>()
      .method("eval", &RCostClass::eval);

    class_<BinarySegmentation>("BinarySegmentation")
      .constructor<SEXP>()
      .method("predict", &BinarySegmentation::predict);
}
