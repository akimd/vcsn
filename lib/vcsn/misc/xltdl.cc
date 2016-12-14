#include <cstdio>
#include <iostream>

#include <boost/filesystem.hpp>

#include <lib/vcsn/misc/xltdl.hh>

namespace vcsn
{
  namespace detail
  {
    namespace
    {
      void init()
      {
        static bool first = true;
        if (first)
          {
            lt_dlinit();
            first = false;
          }
      }
    }

    /*-------------.
    | xlt_advise.  |
    `-------------*/

    xlt_advise::xlt_advise()
      : path_()
    {
      init();
      if (lt_dladvise_init(&advise_))
        raise("failed to initialize dladvise");
    }

    // FIXME: Bad: dtors must not throw.
    xlt_advise::~xlt_advise()
    {
      // FIXME: lt_dlexit when we refcount.
      if (lt_dladvise_destroy(&advise_))
        raise("failed to destroy dladvise");
    }

    xlt_advise&
    xlt_advise::global(bool global)
    {
      if (global ? lt_dladvise_global(&advise_) : lt_dladvise_local(&advise_))
        raise("failed to set dladvise to ", global ? "global" : "local");
      return *this;
    }

    xlt_advise&
    xlt_advise::ext()
    {
      if (lt_dladvise_ext(&advise_))
        raise("failed to set dladvise to ext");
      return *this;
    }

    const file_library&
    xlt_advise::path() const noexcept
    {
      return path_;
    }

    file_library&
    xlt_advise::path() noexcept
    {
      return path_;
    }

    xlt_advise&
    xlt_advise::path(const file_library& p) noexcept
    {
      path_ = p;
      return *this;
    }

    lt_dlhandle
    xlt_advise::dlopen_(const file_library::path& s) const
    {
      return lt_dlopenadvise(s.c_str(), advise_);
    }

    xlt_handle
    xlt_advise::open(const std::string& s)
    {
      // Clear the error flags from previous runs.
      //
      // FIXME: This should be done in libltdl itself.  The problem
      // probably arose from our patches to preserve the first error
      // when traversing the user path: now, even on success, the error
      // flag remains set, and the following run is hindered by it.  We
      // need to complete this patch with a means to ensure that the
      // error flags is restored to its previous state (typically
      // no-error) when eventually we managed to load the file.
      lt_dlerror();

      lt_dlhandle res = nullptr;
      // We cannot simply use search_file in file_library, because we
      // don't know the extension of the file we are looking for (*.la,
      // *.so, *.dyld etc.).  That's an implementation detail that ltdl
      // saves us from.
      using path = file_library::path;
      if (path_.search_path_get().empty() || path(s).is_absolute())
        res = dlopen_(s);
      else
        for (const auto& p: path_.search_path_get())
          if ((res = dlopen_(p / s)))
            break;

      if (!res)
        raise("failed to open ", s);

      return res;
    }


    /*-------------.
    | xlt_handle.  |
    `-------------*/

    xlt_handle::xlt_handle(lt_dlhandle h)
      : handle(h)
    {}

    xlt_handle::~xlt_handle()
    {
      // FIXME: We can't close -- yet.  We need to keep track of the
      // number of trackers.  Otherwise a simple "handle h =
      // advise.open" will close the handle when cleaning the temporary
      // made by "advise.open".

      // close();
    }

    void
    xlt_handle::close()
    {
      if (handle)
        {
          int errors = lt_dlclose(handle);
          handle = nullptr;
          if (errors)
            raise("cannot close");
        }
    }

    void
    xlt_handle::detach()
    {
      assert(handle);
      handle = nullptr;
    }

    void
    xlt_handle::attach(lt_dlhandle h)
    {
      assert(!handle);
      handle = h;
    }


    /*-------------.
    | Standalone.  |
    `-------------*/

    xlt_handle
    xlt_openext(const std::string& s, bool global)
    {
      return xlt_advise().global(global).ext().open(s);
    }
  }
}
