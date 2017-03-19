#pragma once

#include <stdexcept>

#include <ltdl.h>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/file-library.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    class xlt_handle;

    class xlt_advise
    {
    public:
      xlt_advise();
      ~xlt_advise();

      xlt_advise& global(bool global);
      xlt_advise& ext();
      /// Whether to report dlopen attempts.
      xlt_advise& verbose(int v);

      const file_library& path() const noexcept;
      file_library& path() noexcept;
      xlt_advise& path(const file_library& p) noexcept;

      xlt_handle open(const std::string& s);

      /// Throw an exception, or exit with exit_status_ if nonnull.
      template <typename... Args>
      ATTRIBUTE_NORETURN
      static void raise(Args&&... args)
      {
        vcsn::raise(std::forward<Args>(args)...);
      }

    private:
      /// Does not use the search path.  Can return 0.
      lt_dlhandle dlopen_(const file_library::path& s) const;

      lt_dladvise advise_;
      file_library path_;
      int verbose_ = 0;
    };


    class xlt_handle
    {
    public:
      xlt_handle(lt_dlhandle h = nullptr);
      ~xlt_handle();

      /// Close the handle.
      void close();

      /// Detach so that destruction does not close.
      void detach();

      /// Detach so that destruction does not close.
      void attach(lt_dlhandle h);

      /// Wrapper around lt_dlsym that exits on failures.
      template <typename T>
      T sym(const std::string& s)
      {
        assert(handle);
        if (auto res = lt_dlsym(handle, s.c_str()))
          return reinterpret_cast<T>(res);
        else
          raise("failed to dlsym ", s);
      }

      /// The handle.
      /// Exposed, as currently we don't cover the whole lt_ interface.
      lt_dlhandle handle;
    };

    /// Wrapper around lt_dlopenext.
    xlt_handle
    xlt_openext(const std::string& s, bool global);
  }
}
