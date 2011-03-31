/*
 * main.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: sschaet
 */

#include <boost/detail/lightweight_test.hpp>

#include "workspace.hpp"

#include <functional>

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



void workspace_test_basic()
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

void workspace_test_arg()
{
  dtor_counter = 0;
  {
    workspace ws;
    int i = 1;
    dummy & dummy1 = ws.get<dummy>("dummy1", workspace::make_argument(i), 2);

    // get the same object
    dummy & dummy2 = ws.get<dummy>("dummy1", workspace::argument<int>(i), 2);
    BOOST_TEST_EQ(&dummy1.a_, &dummy2.a_);

    // get a different object
    dummy & dummy3 = ws.get<dummy>("dummy3", 1, 2);
    BOOST_TEST(&dummy1.a_ != &dummy3.a_);

    // get yet a different object
    dummy & dummy4 = ws.get<dummy>("dummy1", i, 2);
    BOOST_TEST(&dummy1.a_ != &dummy4.a_);

    // get dummy 1
    dummy & dummy5 = ws.get<dummy>("dummy1", workspace::argument<int>(1), 2);
    BOOST_TEST_EQ(&dummy1.a_, &dummy5.a_);
  }
  // we expect the created objects to be destroyed
  BOOST_TEST_EQ(dtor_counter, 3);
}


int main(void)
{
  workspace_test_basic();
  workspace_test_arg();
  return boost::report_errors();
}
