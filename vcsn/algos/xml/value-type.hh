#ifndef VCSN_ALGOS_XML_VALUE_TYPE_HH
# define VCSN_ALGOS_XML_VALUE_TYPE_HH

# include <vcsn/algos/xml/handlers-base.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/algos/xml/xml-eq.hh>
# include <vcsn/core/rat/ratexpset.hh>

# define EQUAL(NodeName) eq(eq_.NodeName, localname)
# define ATTR_NAME_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getLocalName(I))
# define ATTR_VALUE_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getValue(I))

namespace vcsn
{
  namespace details
  {

    namespace valueType
    {

      template <typename Context>
      class LabelEltChecker : public Handler
      {
      public:
        using context_t = Context;

        LabelEltChecker(parser_t& parser, const context_t& ctx, Handler& father)
          : Handler(parser)
          , ctx_(ctx)
          , father_(father)
        {}

        void start(string, string localname, string, attributes attr) override final
        {
          if (EQUAL(valueSetElt))
            {
              if (attr.getLength() != 1)
                throw std::domain_error("The node valueSetElt need one "
                                        "attribute.");
              if (!ATTR_NAME_EQUAL(value, xsize_t()))
                throw std::domain_error("The node valueSetElt need ane "
                                        "attribute named value");
              if (!ctx_.labelset()->has(transcode(attr.getValue(xsize_t()))[0]))
                throw std::domain_error(std::string("The valueSet has not the letter ")
                                        + transcode(attr.getValue(xsize_t())) + ".");
            }
          else
            throw std::domain_error(std::string("Invalid node, expected "
                                                "valueSetElt, having ")
                                    + transcode(localname) + ".");
        }

        void end(string, string localname, string) override final
        {
          // The attribute father_ is the grandfather, we could not
          // know the number of the element of valueElt inside the
          // valueSet node.
          if(EQUAL(valueSet))
            parser_.setContentHandler(&father_);
        }

      private:
        const context_t& ctx_;
        Handler& father_;
      };

      // Label Set
      template <typename Context>
      class LabelSetChecker : public Handler
      {
      public:
        using context_t = Context;

        LabelSetChecker(parser_t& parser,
                        const context_t& ctx,
                        Handler& father)
          : Handler(parser)
          , ctx_(ctx)
          , father_(father)
          , elt_(parser, ctx, father)
        {}

        void start(string, string localname,
                   string, attributes attr) override final
        {
          if (EQUAL(valueSet))
            {
              if (attr.getLength() != 4)
                throw std::domain_error("The valueSet node need four "
                                        "attributes.");
              bool type = false;
              bool genSort = false;
              bool genKind = false;
              bool genDescript = false;

# define CHECK_REDECLARATION(NAME)                                      \
              if (NAME)                                                 \
                throw std::domain_error("Redeclaration of the attribute " \
                                        #NAME);
              for (xsize_t i = 0; i < attr.getLength(); ++i)
                {
                  if (ATTR_NAME_EQUAL(type, i))
                    {
                      CHECK_REDECLARATION(type);
                      type = ATTR_VALUE_EQUAL(free, i);
                      if (!type)
                        throw std::domain_error("Invalid value for the"
                                                " attribute type of valueSet"
                                                " node.");
                    }
                  else if (ATTR_NAME_EQUAL(genSort, i))
                    {
                      CHECK_REDECLARATION(genSort);
                      genSort = ATTR_VALUE_EQUAL(simple, i);
                      if (!genSort)
                        throw std::domain_error("Invalid value for the"
                                                " attribute"
                                                " genSort of valueSet node.");
                    }
                  else if (ATTR_NAME_EQUAL(genKind, i))
                    {
                      CHECK_REDECLARATION(genKind);
                      genKind = ATTR_VALUE_EQUAL(letter, i);
                      if (!genKind)
                        throw std::domain_error("Invalid value for the"
                                                " attribute"
                                                " genKind of valueSet node.");
                    }
                  else if (ATTR_NAME_EQUAL(genDescript, i))
                    {
                      CHECK_REDECLARATION(genDescript);
                      genDescript = ATTR_VALUE_EQUAL(enumeration, i);

                      if (!genDescript)
                        throw std::domain_error("Invalid value for the"
                                                " attribute genDescript of "
                                                "valueSet node.");
                    }
                }
              parser_.setContentHandler(&elt_);
# undef CHECK_REDECLARATION
            }
          else
            throw std::domain_error(std::string("Unknown node: ")
                                    + transcode(localname)
                                    + ", expected valueSet.");
        }

        void end(string, string, string) override final
        {
          parser_.setContentHandler(&father_);
        }

      private:
        const context_t& ctx_;
        Handler& father_;
        LabelEltChecker<context_t> elt_;
      };

      // Weight Set
      template <typename Context>
      class WeightSetChecker;

      template <typename LabelSet,
                typename SubContext,
                typename Kind,
                template <typename, typename, typename> class Context>
      class WeightSetChecker<Context<LabelSet,
                                     std::shared_ptr<ratexpset<SubContext>>,
                                     Kind>> : public Handler
      {
      public:
        using context_t = Context<LabelSet, std::shared_ptr<ratexpset<SubContext>>, Kind>;
        using sub_context_t = SubContext;
        WeightSetChecker(parser_t& parser, const context_t& ctx, Handler& father)
          : Handler(parser)
          , ctx_(ctx)
          , sub_weightset_checker_(parser, *ctx.weightset(), father)
          , labelset_(parser, ctx, father)
          {}

        virtual void start(string, string localname, string,
                           attributes attr) override final
        {
          if (EQUAL(weightSet))
            {
              // Check the length of the attribute list.
              if (attr.getLength() != 1)
                throw std::domain_error("weighSet node need one "
                                        "attributes when type=series");
              // Check the attribute (type="series").
              if (!ATTR_NAME_EQUAL(type, xsize_t())
                  || !ATTR_VALUE_EQUAL(series, xsize_t()))
                throw std::domain_error(std::string("Bad weightSet attributes. "
                                                    "Need type=\"series\", having ")
                                        + transcode(attr.getLocalName(xsize_t())) + "=\""
                                        + transcode(attr.getValue(xsize_t())) + "\".");
              // Set son handler
              parser_.setContentHandler(sub_weightset_checker_);
            }
          else
            throw std::domain_error(std::string("Unknown node: ")
                                                + transcode(localname)
                                    + ", weightSet expected.");
        }

        virtual void end(string, string, string) override final
        {
          parser_.setContentHandler(labelset_);
        }

      private:
        const context_t& ctx_;
        WeightSetChecker<sub_context_t>& sub_weightset_checker_;
        LabelSetChecker<context_t> labelset_;
      };

      template <typename Context>
      class WeightSetChecker : public Handler
      {
      public:
        using context_t = Context;
        using weightset_t = typename context_t::weightset_t;

        WeightSetChecker(parser_t& parser,
                         const context_t& ctx,
                         Handler& father)
          : Handler(parser)
          , ctx_(ctx)
          , labelSet_(parser, ctx, father)
        {}

# define CHECK_REDECLARATION(NAME)                                      \
              if (NAME)                                                 \
                throw std::domain_error("Redeclaration of the attribute " \
                                        #NAME);

        virtual void start(string, string localname,
                           string, attributes attr) override final
        {
          if (EQUAL(weightSet))
            {
              if (attr.getLength() != 3)
                throw std::domain_error("The node weightSet need three "
                                        "attributes (type, set, and "
                                        "operations)");
              bool type = false;
              bool set = false;
              bool operations = false;

              // Check the attributes
              for (xsize_t i = 0; i < attr.getLength(); ++i)
                {
                  if (ATTR_NAME_EQUAL(type, i))
                    {
                      CHECK_REDECLARATION(type);
                      if (!ATTR_VALUE_EQUAL(numerical, i))
                        throw std::domain_error("Invalid type for weightSet "
                                                "definition, expected "
                                                "type=\"numerical\"");
                      type = true;
                    }
                  else if (ATTR_NAME_EQUAL(operations, i))
                    {
                      CHECK_REDECLARATION(operations);
                      if(!eq(weightset_t::opname(), attr.getValue(i)))
                        throw std::domain_error(std::string("Bad weightset operations."
                                                            " Expected ")
                                                + weightset_t::opname()
                                                + ", having "
                                                + transcode(attr.getValue(i)) + ".");
                      operations = true;
                    }
                  else if (ATTR_NAME_EQUAL(set, i))
                    {
                      CHECK_REDECLARATION(set);
                      if (!eq(weightset_t::sname(), attr.getValue(i)))
                        throw std::domain_error(std::string("Bad weightset. Expected ")
                                                + context_t::weightset_t::sname()
                                                + ", having "
                                                + transcode(attr.getValue(i)) + ".");
                      set = true;
                    }
                  else
                    throw std::domain_error(std::string("Unknown attribute ")
                                            + transcode(attr.getLocalName(i))
                                            + ".");
                }

              // The structure does not have son.
            }
          else
            throw std::domain_error(std::string("Unknown node: ")
                                    + transcode(localname)
                                    + ", weightSet expected.");
        }

# undef CHECK_REDECLARATION

        virtual void end(string, string, string) override final
        {
          parser_.setContentHandler(&labelSet_);
        }
      private:
        const context_t& ctx_;
        LabelSetChecker<context_t> labelSet_;
      };

    } // namespace valueType

    // Value Type checker
    template <typename Context>
    class ValueTypeCheckerHandler;

    // Value Type Element checker
    template <typename Context>
    class ValueSetElementHandler : public Handler
    {
    public:
      using context_t = Context;

      ValueSetElementHandler(const context_t& ctx,
                             parser_t& parser,
                             Handler& father)
        : Handler(parser)
        , ctx_(ctx)
        , father_(father)
      {}

      virtual void start(string,
                         string localname,
                         string,
                         attributes attr) override final
      {
        if (!EQUAL(valueSetElt))
          throw std::domain_error(std::string("Unknown node: ")
                                  + transcode(localname));

        if (attr.getLength() != 1)
          throw std::domain_error("Need value attribute for valueSetElt");

        if (!ctx_.labelset()->has(transcode(attr.getValue(XMLSize_t()))[0])
            && strlen(transcode(attr.getValue(XMLSize_t()))) == 1)
          throw std::domain_error(std::string("unknown label element in"
                                              "valueType definition: ")
                                  + transcode(attr.getValue(XMLSize_t())));
      }

      virtual void end(string, string localname, string) override final
      {
        if (EQUAL(valueType))
          parser_.setContentHandler(&father_);
      }

    private:
      const context_t& ctx_;
      Handler& father_;
    };

    // Handler for valueType check with static defined context.
    template <typename Context, typename StructureHandler>
    class StaticContentHandler : public Handler
    {
    public:
      using context_t = Context;
      using structure_t = StructureHandler;

      using content_t = typename structure_t::content_t;

      StaticContentHandler(const context_t& ctx,
                           parser_t& parser,
                           Handler& father)
        : Handler(parser)
        , ctx_(ctx)
        , weightSetHandler_(parser, ctx, *this)
        , valueType_(false)
        , structure_(false)
        , struct_handler_(ctx, parser, *this)
        , father_(father)
      {}

      virtual void start(string,
                         string localname,
                         string,
                         attributes attr) override final
      {
        if (EQUAL(valueType))
          {
            if (valueType_)
              throw std::domain_error("Redeclaration of valueType node.");
            if (attr.getLength())
              throw std::domain_error("valueType node must have zero"
                                      " attribute.");
            parser_.setContentHandler(&weightSetHandler_);
            valueType_ = true;
          }
        else if (eq(structure_t::type(), localname))
          {
            if (structure_)
              throw std::domain_error(std::string("Redeclaration of the ")
                                      + structure_t::type() + " node.");
            if (!valueType_)
              throw std::domain_error(std::string("The valueType node must be"
                                                  " defined before the ")
                                      + structure_t::type() + " node.");
            parser_.setContentHandler(&struct_handler_);
            structure_ = true;
          }
        else
          throw
            std::domain_error(std::string("Unknown ") + transcode(localname)
                              + " node.");
      }

      virtual void end(string, string localname, string) override final
      {
        if (eq(structure_t::type(), localname))
          parser_.setContentHandler(&father_);
      }

      content_t get_parsed_content()
      {
        if (!structure_)
          throw std::domain_error(std::string("Has not read the ")
                                  + structure_t::type() + " node.");
        return struct_handler_.get_parsed_content();
      }

    private:
      const context_t& ctx_;
      valueType::WeightSetChecker<context_t> weightSetHandler_;
      bool valueType_;
      bool structure_;
      structure_t struct_handler_;
      Handler& father_;
    };

  } // namespace details
} // namespace vcsn

# undef ATTR_NAME_EQUAL
# undef ATTR_VALUE_EQUAL
# undef EQUAL

#endif // !VCSN_ALGOS_XML_VALUE_TYPE_HH
