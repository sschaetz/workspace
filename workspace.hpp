#ifndef WORKSPACE_HPP
#define WORKSPACE_HPP

/**
 * @file workspace.hpp
 *
 * This header contains the workspace class which provides means of maintaining
 * a number of reusable objects across multiple algorithm invocations.
 */

#include <map>
#include <vector>

#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>


/**
 * @brief workspace class that optimizes resource management across consecutive
 * calls to the same function.
 *
 * @ingroup main
 */
class workspace
{
  private:

    /**
     * @brief one workspace entry
     */
    struct entry
    {
      unsigned int num_;

      void * ptr_;

      boost::function<void (void * ptr)> dtor_;

      entry(void * ptr, boost::function<void (void * ptr)> dtor,
        unsigned int num) :
        num_(num), ptr_(ptr), dtor_(dtor)
      {}
    };

    typedef std::pair<std::string, entry> data_entry_type;
    typedef std::map<std::string, entry> data_type;
    typedef data_type::iterator data_iterator_type;

    /// specifies if workspace should be used at all
    bool use_;

    /// the workspace data
    data_type data;


  public:

    /**
     * @brief default constructor
     */
    workspace() {}

    /**
     * @brief constructor that allows specification if workspace should be used
     *
     * @param use should workspace be used
     */
    workspace(bool use) : use_(use) {}

    /**
     * @brief destructor destorys all objects in the workspace
     *
     * Workspace destructor destroys object in reverse order of construction
     */
    ~workspace()
    {
      data_iterator_type it;
      // destroy sorted
      std::vector<data_iterator_type> buffer(data.size());
      for(it = data.begin(); it!=data.end(); ++it)
      {
        buffer[data.size()-it->second.num_ -1] = it;
      }

      std::vector<data_iterator_type>::iterator i;
      for(i = buffer.begin(); i!=buffer.end(); ++i)
      {
        (*i)->second.dtor_((*i)->second.ptr_);
      }
    }

    /**
     * @brief return use flag
     *
     * @return value of use flag
     */
    inline bool use() const { return use_; }


    /**
     * @brief access or create element of workspace
     *
     * @param name name of element
     */
    template<typename T>
    T & get(const std::string & name)
    {
      data_iterator_type it = data.find(name);
      if(it == data.end())
      {
        T* ptr = new T();
        boost::function<void (void * ptr)> dtor =
          boost::lambda::bind(boost::lambda::delete_ptr(),
            boost::lambda::ll_reinterpret_cast<T*>(boost::lambda::_1));
        data.insert(data_entry_type(name, entry(ptr, dtor, data.size())));
        return *ptr;
      }
      else
      {
        return *((T*)(it->second.ptr_));
      }
    }

    template<typename T, typename T0>
    T & get(const std::string & name, T0 p0)
    {
      data_iterator_type it = data.find(name);
      if(it == data.end())
      {
        T* ptr = new T(p0);
        boost::function<void (void * ptr)> dtor =
          boost::lambda::bind(boost::lambda::delete_ptr(),
            boost::lambda::ll_reinterpret_cast<T*>(boost::lambda::_1));
        data.insert(data_entry_type(name, entry(ptr, dtor, data.size())));
        return *ptr;
      }
      else
      {
        return *((T*)(it->second.ptr_));
      }
    }

    template<typename T, typename T0, typename T1>
    T & get(const std::string & name, T0 p0, T1 p1)
    {
      data_iterator_type it = data.find(name);
      if(it == data.end())
      {
        T* ptr = new T(p0, p1);
        boost::function<void (void * ptr)> dtor =
          boost::lambda::bind(boost::lambda::delete_ptr(),
            boost::lambda::ll_reinterpret_cast<T*>(boost::lambda::_1));
        data.insert(data_entry_type(name, entry(ptr, dtor, data.size())));
        return *ptr;
      }
      else
      {
        return *((T*)(it->second.ptr_));
      }
    }

    template<typename T, typename T0, typename T1, typename T2>
    T & get(const std::string & name, T0 p0, T1 p1, T2 p2)
    {
      data_iterator_type it = data.find(name);
      if(it == data.end())
      {
        T* ptr = new T(p0, p1, p2);
        boost::function<void (void * ptr)> dtor =
          boost::lambda::bind(boost::lambda::delete_ptr(),
            boost::lambda::ll_reinterpret_cast<T*>(boost::lambda::_1));
        data.insert(data_entry_type(name, entry(ptr, dtor, data.size())));
        return *ptr;
      }
      else
      {
        return *((T*)(it->second.ptr_));
      }
    }
};



#endif // WORKSPACE_HPP
