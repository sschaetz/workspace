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
#include <functional>
#include <string>

/**
 * @brief workspace class that optimizes resource management across consecutive
 * calls to the same function.
 *
 * @ingroup main
 */
class workspace
{
  public:

    template <typename ArgType>
    struct special_arg
    {
      special_arg(const ArgType & arg) : arg_(arg) {}
      ArgType arg_;
    };

    template <typename ArgType>
    static special_arg<ArgType> arg(const ArgType & arg)
    {
      return special_arg<ArgType>(arg);
    }

  private:

    /**
     * @brief one workspace entry
     */
    struct entry
    {
      // number, ensure destruction in reverse order of construction
      unsigned int num_;
      // pointer to object
      void * ptr_;
      // function that can properly destroy the object
      std::function<void (void *&)> dtor_;

      // class constructor
      entry(void * ptr, std::function<void (void *&)> dtor, unsigned int num) :
        num_(num), ptr_(ptr), dtor_(dtor) {}
    };

    typedef std::pair<std::string, entry> map_entry_type;
    typedef std::map<std::string, entry> map_type;
    typedef map_type::iterator map_iterator_type;

    /// specifies if workspace should be used at all
    bool use_;

    /// the workspace data
    map_type storage;

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
      map_iterator_type it;
      // destroy sorted
      std::vector<map_iterator_type> buffer(storage.size());
      for(it = storage.begin(); it!=storage.end(); ++it)
      {
        buffer[storage.size()-it->second.num_ -1] = it;
      }

      std::vector<map_iterator_type>::iterator i;
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
    template<typename T, typename... Args>
    T & get(std::string name, const Args&... args)
    {
      // reserve space in the string to speed up append operation
      std::size_t size = name.size();
      param_size(size, args...);
      name.reserve(size);

      // append each parameter that was passes as argument to the string
      param_append(name, args...);

      // check if we already have this object
      map_iterator_type it = storage.find(name);
      if(it == storage.end())
      {
        // we don't have the object, create a new one
        T* ptr = new T( unwrap_argument<Args>()(args)... );

        // and insert it into the map
        storage.insert(
          map_entry_type(
            name,
            entry(
              ptr,
              [](void *& ptr){ delete reinterpret_cast<T*>(ptr); },
              storage.size()
            )
          )
        );
        // finally return the created object as reference
        return *ptr;
      }
      else
      {
        // we have the object already, return as reference
        return *((T*)(it->second.ptr_));
      }
    }



  private:

    // deduce size of all arguments ____________________________________________

    inline void param_size(std::size_t & size) {}

    template<typename T, typename... Args>
    inline void param_size(std::size_t & size, special_arg<T> param,
      Args... args)
    {
      size += sizeof(T);
      param_size(size, args...);
    }

    template<typename T, typename... Args>
    inline void param_size(std::size_t & size, T param, Args... args)
    {
      param_size(size, args...);
    }

    // create string from arguments for insertion in hash ______________________

    inline void param_append(std::string & str) {}

    template<typename T, typename... Args>
    inline void param_append(std::string & str, special_arg<T> param,
      Args... args)
    {
      str.append((const char *)&param, sizeof(T));
      param_append(str, args...);
    }

    template<typename T, typename... Args>
    inline void param_append(std::string & str, T param, Args... args)
    {
      param_append(str, args...);
    }

    // unwrap passed arguments _________________________________________________

    template <typename T>
    struct unwrap_argument
    {
      inline T operator() (T val)
      {
        return val;
      }
    };

    template <typename T>
    struct unwrap_argument<special_arg<T> >
    {
      inline T operator() (special_arg<T> val)
      {
        return val.arg_;
      }
    };

};



#endif // WORKSPACE_HPP
