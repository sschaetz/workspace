/*
 * main.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: sschaet
 */

#include <boost/detail/lightweight_test.hpp>

#include "workspace.hpp"

static int dtor_counter;

struct dummy
{
  int a_, b_;
  float c_;
  double d_;

  dummy()                                : a_(-1), b_(-1), c_(-1.), d_(-1.) {}
  dummy(int a)                           : a_(a), b_(-1), c_(-1.), d_(-1.) {}
  dummy(int a, int b)                    : a_(a), b_(b), c_(-1.), d_(-1.) {}
  dummy(int a, int b, float c)           : a_(a), b_(b), c_(c), d_(-1.) {}
  dummy(int a, int b, float c, double d) : a_(a), b_(b), c_(c), d_(d) {}

  ~dummy() { dtor_counter++; }

};

void test1()
{
  dtor_counter = 0;
  {
    workspace ws;
    dummy & dummy1 = ws.get<dummy>("dummy1", 1, 2);

    // get the same object
    dummy & dummy2 = ws.get<dummy>("dummy1", 1, 2);
    BOOST_TEST_EQ(&dummy1.a_, &dummy2.a_);

    // get a different object
    dummy & dummy3 = ws.get<dummy>("dummy3", 1, 2);
    BOOST_TEST(&dummy1.a_ != &dummy3.a_);
  }
  // we expect the created objects to be destroyed
  BOOST_TEST_EQ(dtor_counter, 2);
}


int main(void)
{
  test1();

  boost::report_errors();
}
