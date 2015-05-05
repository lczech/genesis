Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator->
Unknown operator: operator=
Unknown operator: operator()
Unknown operator: operator()
Unknown operator: operator=
Unknown operator: operator=
Unknown operator: operator=
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator++
Unknown operator: operator=
Unknown operator: operator=
Unknown operator: operator++
Unknown operator: operator++

// =============================================================================
//     Namespace genesis
// =============================================================================


// =============================================================================
//     Classes
// =============================================================================


    // -------------------------------------------------------------------
    //     Class Bipartition
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Bipartition > ( "Bipartition", boost::python::init< size_t >(( boost::python::arg("num_leaves") )) )

        // Public Member Functions

        .def(
            "Link",
            ( const LinkType * ( ::genesis::Bipartition::* )(  ))( &::genesis::Bipartition::Link )
        )
        .def(
            "Invert",
            ( void ( ::genesis::Bipartition::* )(  ))( &::genesis::Bipartition::Invert )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Bipartitions
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Bipartitions > ( "Bipartitions", boost::python::init< const TreeType * >(( boost::python::arg("tree") )) )

        // Public Member Functions

        .def(
            "Make",
            ( void ( ::genesis::Bipartitions::* )(  ))( &::genesis::Bipartitions::Make )
        )
        .def(
            "MakeIndex",
            ( void ( ::genesis::Bipartitions::* )(  ))( &::genesis::Bipartitions::MakeIndex )
        )
        .def(
            "FindSmallestSubtree",
            ( BipartitionType * ( ::genesis::Bipartitions::* )( std::vector< NodeType * > ))( &::genesis::Bipartitions::FindSmallestSubtree ),
            ( boost::python::arg("nodes") )
        )
        .def(
            "GetSubtreeEdges",
            ( std::vector< const EdgeType * > ( ::genesis::Bipartitions::* )( const LinkType * ))( &::genesis::Bipartitions::GetSubtreeEdges ),
            ( boost::python::arg("subtree") )
        )
        .def(
            "Validate",
            ( bool ( ::genesis::Bipartitions::* )(  ))( &::genesis::Bipartitions::Validate )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Bipartitions::* )(  ))( &::genesis::Bipartitions::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Bitvector
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Bitvector > ( "Bitvector", boost::python::init< const size_t, boost::python::optional< const bool > >(( boost::python::arg("size"), boost::python::arg("init")=(const bool)(false) )) )
        .def( boost::python::init< const size_t, const std::initializer_list< int > >(( boost::python::arg("size"), boost::python::arg("list") )) )

        // Public Member Functions

        .def(
            "size",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::size ),
            "Returns the size (number of total bits) of this Bitvector."
        )
        .def(
            "Get",
            ( bool ( ::genesis::Bitvector::* )( size_t ) const )( &::genesis::Bitvector::Get ),
            ( boost::python::arg("index") ),
            "Returns the value of a single bit, with boundary check."
        )
        .def(
            "Set",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::Set ),
            ( boost::python::arg("index") ),
            "Sets the value of a single bit to true, with boundary check."
        )
        .def(
            "Unset",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::Unset ),
            ( boost::python::arg("index") ),
            "Sets the value of a single bit to false, with boundary check."
        )
        .def(
            "Set",
            ( void ( ::genesis::Bitvector::* )( size_t, bool ))( &::genesis::Bitvector::Set ),
            ( boost::python::arg("index"), boost::python::arg("value") ),
            "Sets the value of a single bit to a given bool value, with boundary check."
        )
        .def(
            "Flip",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::Flip ),
            ( boost::python::arg("index") ),
            "Flips (inverts) the value of a single bit, with boundary check."
        )
        .def(
            "SymmetricDifference",
            ( Bitvector ( ::genesis::Bitvector::* )( Bitvector const & ) const )( &::genesis::Bitvector::SymmetricDifference ),
            ( boost::python::arg("rhs") )
        )
        .def(
            "Count",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::Count ),
            "Counts the number of set bits in the Bitvector."
        )
        .def(
            "Hash",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::Hash ),
            "Returns an std::hash value for the Bitvector."
        )
        .def(
            "XHash",
            ( IntType ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::XHash ),
            "Returns a hash value of type IntType, that is quicker to calculate than Hash(), and thus can be used where the std::hash is not needed."
        )
        .def(
            "Invert",
            ( void ( ::genesis::Bitvector::* )(  ))( &::genesis::Bitvector::Invert ),
            "Flip all bits."
        )
        .def(
            "Normalize",
            ( void ( ::genesis::Bitvector::* )(  ))( &::genesis::Bitvector::Normalize ),
            "Brings the Bitvector in a normalized form, where the first bit is always zero."
        )
        .def(
            "Reset",
            ( void ( ::genesis::Bitvector::* )( bool ))( &::genesis::Bitvector::Reset ),
            ( boost::python::arg("value")=(bool)(false) ),
            "Reset all the bits to false. If provided with parameter true, sets all bits to true."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::Dump )
        )
        .def(
            "DumpInt",
            ( std::string ( ::genesis::Bitvector::* )( IntType ) const )( &::genesis::Bitvector::DumpInt ),
            ( boost::python::arg("x") )
        )
        .def(
            "SymmetricDifference",
            ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::Bitvector::SymmetricDifference ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs") )
        )
        .staticmethod("SymmetricDifference")

        // Operators

        .def(
            "__getitem__",
            ( bool ( ::genesis::Bitvector::* )( size_t ) const )( &::genesis::Bitvector::operator[] ),
            ( boost::python::arg("index") ),
            "Returns the value of a single bit, without boundary check."
        )
        .def( boost::python::self &= boost::python::self )
        .def( boost::python::self |= boost::python::self )
        .def( boost::python::self ^= boost::python::self )
        .def( ~boost::python::self )
        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
        .def( boost::python::self < boost::python::self )
        .def( boost::python::self > boost::python::self )
        .def( boost::python::self <= boost::python::self )
        .def( boost::python::self >= boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class DefaultEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::DefaultEdgeData > ( "DefaultEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::DefaultEdgeData::* )( NewickBrokerElement * ))( &::genesis::DefaultEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") ),
            "Fills the edge with data from a NewickBrokerElement."
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::DefaultEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::DefaultEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::DefaultEdgeData::* )(  ) const )( &::genesis::DefaultEdgeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class DefaultNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::DefaultNodeData > ( "DefaultNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::DefaultNodeData::* )( NewickBrokerElement * ))( &::genesis::DefaultNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") ),
            "Fills the node with data from a NewickBrokerElement."
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::DefaultNodeData::* )( NewickBrokerElement * ) const )( &::genesis::DefaultNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::DefaultNodeData::* )(  ) const )( &::genesis::DefaultNodeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class FastaLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::FastaLexer > ( "FastaLexer", boost::python::init<  >(  ) )
    ;

    // -------------------------------------------------------------------
    //     Class FastaProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::FastaProcessor > ( "FastaProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( * )( const std::string, SequenceSet & ))( &::genesis::FastaProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, SequenceSet & ))( &::genesis::FastaProcessor::FromString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( * )( const std::string, const SequenceSet & ))( &::genesis::FastaProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const SequenceSet & ))( &::genesis::FastaProcessor::ToString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const SequenceSet & ))( &::genesis::FastaProcessor::ToString ),
            ( boost::python::arg("aln") )
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class JplaceProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "GetVersion",
            ( std::string ( * )(  ))( &::genesis::JplaceProcessor::GetVersion ),
            "Returns the version number that this class is written for."
        )
        .staticmethod("GetVersion")
        .def(
            "CheckVersion",
            ( bool ( * )( const std::string ))( &::genesis::JplaceProcessor::CheckVersion ),
            ( boost::python::arg("version") ),
            "Checks whether the version of the jplace format works with this parser."
        )
        .staticmethod("CheckVersion")
        .def(
            "FromFile",
            ( bool ( * )( const std::string &, PlacementMap & ))( &::genesis::JplaceProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, PlacementMap & ))( &::genesis::JplaceProcessor::FromString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromString")
        .def(
            "FromDocument",
            ( bool ( * )( const JsonDocument &, PlacementMap & ))( &::genesis::JplaceProcessor::FromDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") ),
            "Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromDocument")
        .def(
            "ToFile",
            ( bool ( * )( const std::string, const PlacementMap & ))( &::genesis::JplaceProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const PlacementMap & ))( &::genesis::JplaceProcessor::ToString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const PlacementMap & ))( &::genesis::JplaceProcessor::ToString ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToDocument",
            ( void ( * )( JsonDocument &, const PlacementMap & ))( &::genesis::JplaceProcessor::ToDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") )
        )
        .staticmethod("ToDocument")
    ;

    // -------------------------------------------------------------------
    //     Class JsonDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonDocument > ( "JsonDocument", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( ::genesis::JsonDocument::* )(  ) const )( &::genesis::JsonDocument::ToString )
        )
        .def(
            "Validate",
            ( bool ( ::genesis::JsonDocument::* )(  ))( &::genesis::JsonDocument::Validate )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::JsonDocument::* )(  ) const )( &::genesis::JsonDocument::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonLexer > ( "JsonLexer", boost::python::init<  >(  ) )
    ;

    // -------------------------------------------------------------------
    //     Class JsonProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonProcessor > ( "JsonProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( * )( const std::string &, JsonDocument & ))( &::genesis::JsonProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Takes a JSON document file path and parses its contents into a JsonDocument."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, JsonDocument & ))( &::genesis::JsonProcessor::FromString ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            "Takes a string containing a JSON document and parses its contents into a JsonDocument."
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( * )( const std::string &, const JsonDocument & ))( &::genesis::JsonProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Writes a Json file from a JsonDocument. Returns true iff successful."
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const JsonDocument & ))( &::genesis::JsonProcessor::ToString ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            "Gives the Json string representation of a JsonDocument."
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const JsonDocument & ))( &::genesis::JsonProcessor::ToString ),
            ( boost::python::arg("document") ),
            "Returns the Json representation of a JsonDocument."
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class JsonValue
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValue > ( "JsonValue" )

        // Public Member Functions

        .def(
            "TypeToString",
            ( std::string ( * )( const Type ))( &::genesis::JsonValue::TypeToString ),
            ( boost::python::arg("t") )
        )
        .staticmethod("TypeToString")
        .def(
            "TypeToString",
            ( std::string ( ::genesis::JsonValue::* )(  ) const )( &::genesis::JsonValue::TypeToString )
        )
        .def(
            "type",
            ( Type ( ::genesis::JsonValue::* )(  ) const )( &::genesis::JsonValue::type )
        )
        .def(
            "IsNull",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsNull )
        )
        .def(
            "IsBool",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsBool )
        )
        .def(
            "IsNumber",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsNumber )
        )
        .def(
            "IsString",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsString )
        )
        .def(
            "IsArray",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsArray )
        )
        .def(
            "IsObject",
            ( bool ( ::genesis::JsonValue::* )(  ))( &::genesis::JsonValue::IsObject )
        )
        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValue::* )(  ) const )( &::genesis::JsonValue::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueArray
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueArray > ( "JsonValueArray", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "cbegin",
            ( const_iterator ( ::genesis::JsonValueArray::* )(  ) const )( &::genesis::JsonValueArray::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::JsonValueArray::* )(  ) const )( &::genesis::JsonValueArray::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( JsonValue * ( ::genesis::JsonValueArray::* )( const std::size_t ) const )( &::genesis::JsonValueArray::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the array, doing a boundary check first."
        )
        .def(
            "empty",
            ( bool ( ::genesis::JsonValueArray::* )(  ) const )( &::genesis::JsonValueArray::empty ),
            "Returns whether the array is empty."
        )
        .def(
            "size",
            ( size_t ( ::genesis::JsonValueArray::* )(  ) const )( &::genesis::JsonValueArray::size ),
            "Returns the number of values in the array."
        )
        .def(
            "push_back",
            ( void ( ::genesis::JsonValueArray::* )( JsonValue * ))( &::genesis::JsonValueArray::push_back ),
            ( boost::python::arg("value") )
        )
        .def(
            "Add",
            ( void ( ::genesis::JsonValueArray::* )( JsonValue * ))( &::genesis::JsonValueArray::Add ),
            ( boost::python::arg("value") ),
            "Alias of push_back()."
        )
        .def(
            "pop_back",
            ( void ( ::genesis::JsonValueArray::* )(  ))( &::genesis::JsonValueArray::pop_back )
        )
        .def(
            "clear",
            ( void ( ::genesis::JsonValueArray::* )(  ))( &::genesis::JsonValueArray::clear ),
            "Clears all values, as if the array was newly created."
        )
        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueArray::* )(  ) const )( &::genesis::JsonValueArray::ToString )
        )

        // Operators

        .def(
            "__getitem__",
            ( JsonValue * ( ::genesis::JsonValueArray::* )( const std::size_t ) const )( &::genesis::JsonValueArray::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the array."
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::JsonValueArray::begin, &::genesis::JsonValueArray::end )
        )
        .add_property(
            "",
            boost::python::range ( &::genesis::JsonValueArray::begin, &::genesis::JsonValueArray::end )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueBool
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueBool > ( "JsonValueBool", boost::python::init<  >(  ) )
        .def( boost::python::init< const bool >(( boost::python::arg("v") )) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueBool::* )(  ) const )( &::genesis::JsonValueBool::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueNull
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueNull > ( "JsonValueNull", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueNull::* )(  ) const )( &::genesis::JsonValueNull::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueNumber
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueNumber > ( "JsonValueNumber", boost::python::init<  >(  ) )
        .def( boost::python::init< const double >(( boost::python::arg("v") )) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueNumber::* )(  ) const )( &::genesis::JsonValueNumber::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueObject
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueObject > ( "JsonValueObject", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "cbegin",
            ( const_iterator ( ::genesis::JsonValueObject::* )(  ) const )( &::genesis::JsonValueObject::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::JsonValueObject::* )(  ) const )( &::genesis::JsonValueObject::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( JsonValue * ( ::genesis::JsonValueObject::* )( const std::string & ) const )( &::genesis::JsonValueObject::at ),
            ( boost::python::arg("name") ),
            "Provides index based array access to the object, doing a boundary check first. This is an alias for Get()."
        )
        .def(
            "empty",
            ( bool ( ::genesis::JsonValueObject::* )(  ) const )( &::genesis::JsonValueObject::empty ),
            "Returns whether the object is empty."
        )
        .def(
            "size",
            ( size_t ( ::genesis::JsonValueObject::* )(  ) const )( &::genesis::JsonValueObject::size ),
            "Returns the number of values in the object."
        )
        .def(
            "Has",
            ( bool ( ::genesis::JsonValueObject::* )( const std::string & ) const )( &::genesis::JsonValueObject::Has ),
            ( boost::python::arg("name") ),
            "Returns true iff the object contains a certain key."
        )
        .def(
            "Get",
            ( JsonValue * ( ::genesis::JsonValueObject::* )( const std::string & ) const )( &::genesis::JsonValueObject::Get ),
            ( boost::python::arg("name") ),
            "Returns the value of a certain key if present in the object, nullptr otherwise."
        )
        .def(
            "clear",
            ( void ( ::genesis::JsonValueObject::* )(  ))( &::genesis::JsonValueObject::clear ),
            "Clears all values, as if the object was newly created."
        )
        .def(
            "erase",
            ( bool ( ::genesis::JsonValueObject::* )( const std::string ))( &::genesis::JsonValueObject::erase ),
            ( boost::python::arg("name") ),
            "Clears the value of a specific key; returns true iff this key existed."
        )
        .def(
            "Set",
            ( void ( ::genesis::JsonValueObject::* )( const std::string &, JsonValue * ))( &::genesis::JsonValueObject::Set ),
            ( boost::python::arg("name"), boost::python::arg("value") ),
            "Sets the value for a certain key."
        )
        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueObject::* )(  ) const )( &::genesis::JsonValueObject::ToString )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::JsonValueObject::begin, &::genesis::JsonValueObject::end )
        )
        .add_property(
            "",
            boost::python::range ( &::genesis::JsonValueObject::begin, &::genesis::JsonValueObject::end )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueString
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JsonValueString > ( "JsonValueString", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( ::genesis::JsonValueString::* )(  ) const )( &::genesis::JsonValueString::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Lexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Lexer > ( "Lexer" )

        // Public Member Functions

        .def(
            "ProcessFile",
            ( bool ( ::genesis::Lexer::* )( const std::string & ))( &::genesis::Lexer::ProcessFile ),
            ( boost::python::arg("fn") ),
            "Shortcut function that reads the contents of a file and then calls ProcessString()."
        )
        .def(
            "ProcessString",
            ( bool ( ::genesis::Lexer::* )( const std::string &, bool ))( &::genesis::Lexer::ProcessString ),
            ( boost::python::arg("text"), boost::python::arg("stepwise")=(bool)(false) ),
            "Process a string and store the resulting tokens in this Lexer object."
        )
        .def(
            "ProcessStep",
            ( bool ( ::genesis::Lexer::* )(  ))( &::genesis::Lexer::ProcessStep ),
            "Processes one step of the lexing."
        )
        .def(
            "ValidateBrackets",
            ( bool ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::ValidateBrackets ),
            "Checkes whether the bracket tokes are validly nested."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::Dump ),
            "Returns a listing of the parse result in readable form."
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( LexerToken ( ::genesis::Lexer::* )( const std::size_t ) const )( &::genesis::Lexer::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the tokens, doing a boundary check first."
        )
        .def(
            "front",
            ( LexerToken ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::front ),
            "Returns a reference to the first token."
        )
        .def(
            "back",
            ( LexerToken ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::back ),
            "Returns a reference to the last token."
        )
        .def(
            "empty",
            ( bool ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::empty ),
            "Returns whether the list of tokens is empty."
        )
        .def(
            "size",
            ( size_t ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::size ),
            "Returns the number of tokens produced during the analysis process."
        )
        .def(
            "clear",
            ( void ( ::genesis::Lexer::* )(  ))( &::genesis::Lexer::clear ),
            "Clears all tokens, as if the object was newly created."
        )
        .def(
            "HasError",
            ( bool ( ::genesis::Lexer::* )(  ) const )( &::genesis::Lexer::HasError ),
            "Returns whether there appeared an error while lexing."
        )

        // Operators

        .def(
            "__getitem__",
            ( LexerToken ( ::genesis::Lexer::* )( const std::size_t ) const )( &::genesis::Lexer::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the tokens."
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::Lexer::begin, &::genesis::Lexer::end )
        )
        .add_property(
            "",
            boost::python::range ( &::genesis::Lexer::begin, &::genesis::Lexer::end )
        )
    ;

    // -------------------------------------------------------------------
    //     Class LexerIterator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::LexerIterator > ( "LexerIterator", boost::python::init< Lexer &, int >(( boost::python::arg("lexer"), boost::python::arg("position") )) )

        // Public Member Functions

        .def(
            "ConsumeWithTail",
            ( void ( ::genesis::LexerIterator::* )( const int ))( &::genesis::LexerIterator::ConsumeWithTail ),
            ( boost::python::arg("tail_size") ),
            "Determines the consumption policy of the iterator when traversion the Lexer."
        )
        .def(
            "ProduceWithHead",
            ( void ( ::genesis::LexerIterator::* )( const int ))( &::genesis::LexerIterator::ProduceWithHead ),
            ( boost::python::arg("head_size") ),
            "Determines the production policy of the iterator when traversion the Lexer."
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class Logging
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Logging > ( "Logging", boost::python::init<  >(  ) )
        .def( boost::python::init< const Logging & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "Get",
            ( std::ostringstream & ( ::genesis::Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel ))( &::genesis::Logging::Get ),
            ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level") ),
            "Getter for the singleton instance of log, is called by the standard macros."
        )
        .def(
            "Get",
            ( std::ostringstream & ( ::genesis::Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel, const LoggingDetails ))( &::genesis::Logging::Get ),
            ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level"), boost::python::arg("dets") ),
            "Getter for the singleton instance of log, is called by special macros that change the details of the log message."
        )
        .def(
            "AddOutputStream",
            ( void ( * )( std::ostream & ))( &::genesis::Logging::AddOutputStream ),
            ( boost::python::arg("os") ),
            "Add an output stream to which log messages are written."
        )
        .staticmethod("AddOutputStream")
        .def(
            "AddOutputFile",
            ( void ( * )( const std::string & ))( &::genesis::Logging::AddOutputFile ),
            ( boost::python::arg("fn") ),
            "Add an output file to which log messages are written."
        )
        .staticmethod("AddOutputFile")
        .def(
            "max_level",
            ( LoggingLevel ( * )(  ))( &::genesis::Logging::max_level ),
            "Get the highest log level that is reported."
        )
        .staticmethod("max_level")
        .def(
            "max_level",
            ( void ( * )( const LoggingLevel ))( &::genesis::Logging::max_level ),
            ( boost::python::arg("level") ),
            "Set the highest log level that is reported."
        )
        .staticmethod("max_level")
        .def(
            "report_percentage",
            ( int ( * )(  ))( &::genesis::Logging::report_percentage ),
            "Get the current percentage for reporting LOG_PROG messages."
        )
        .staticmethod("report_percentage")
        .def(
            "report_percentage",
            ( void ( * )( const int ))( &::genesis::Logging::report_percentage ),
            ( boost::python::arg("percentage") ),
            "set the percentage for reporting LOG_PROG messages."
        )
        .staticmethod("report_percentage")
        .def(
            "LevelToString",
            ( std::string ( * )( const LoggingLevel ))( &::genesis::Logging::LevelToString ),
            ( boost::python::arg("level") ),
            "Return a string representation of a log level."
        )
        .staticmethod("LevelToString")

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class Matrix
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Matrix > ( "Matrix", boost::python::init< size_t, size_t >(( boost::python::arg("rows"), boost::python::arg("cols") )) )
        .def( boost::python::init< size_t, size_t, value_type >(( boost::python::arg("rows"), boost::python::arg("cols"), boost::python::arg("init") )) )

        // Public Member Functions

        .def(
            "Rows",
            ( size_t ( ::genesis::Matrix::* )(  ) const )( &::genesis::Matrix::Rows )
        )
        .def(
            "Cols",
            ( size_t ( ::genesis::Matrix::* )(  ) const )( &::genesis::Matrix::Cols )
        )
        .def(
            "size",
            ( size_t ( ::genesis::Matrix::* )(  ) const )( &::genesis::Matrix::size )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Matrix::* )(  ))( &::genesis::Matrix::Dump )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class NewickBroker
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::NewickBroker > ( "NewickBroker" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::clear ),
            "Deletes all nodes from the broker."
        )
        .def(
            "PushTop",
            ( void ( ::genesis::NewickBroker::* )( NewickBrokerElement * ))( &::genesis::NewickBroker::PushTop ),
            ( boost::python::arg("node") )
        )
        .def(
            "PushBottom",
            ( void ( ::genesis::NewickBroker::* )( NewickBrokerElement * ))( &::genesis::NewickBroker::PushBottom ),
            ( boost::python::arg("node") )
        )
        .def(
            "PopTop",
            ( void ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::PopTop )
        )
        .def(
            "PopBottom",
            ( void ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::PopBottom )
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::cend ),
            "Const version of end()."
        )
        .def(
            "rbegin",
            ( reverse_iterator ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::rbegin ),
            "Returns a reverse iterator to the nodes on the stack."
        )
        .def(
            "rend",
            ( reverse_iterator ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::rend ),
            "Reverse version of end()."
        )
        .def(
            "crbegin",
            ( const_reverse_iterator ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::crbegin ),
            "Const version of rbegin()."
        )
        .def(
            "crend",
            ( const_reverse_iterator ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::crend ),
            "Const version of rend()."
        )
        .def(
            "empty",
            ( bool ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::empty ),
            "Returns whether the stack is empty."
        )
        .def(
            "size",
            ( size_t ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::size ),
            "Returns the size of the stack, i.e. the number of nodes stored in the broker."
        )
        .def(
            "at",
            ( NewickBrokerElement * ( ::genesis::NewickBroker::* )( const std::size_t ) const )( &::genesis::NewickBroker::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the nodes, doing a boundary check first."
        )
        .def(
            "Top",
            ( NewickBrokerElement * ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::Top ),
            "Returns a reference to the top node of the tree stack."
        )
        .def(
            "Bottom",
            ( NewickBrokerElement * ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::Bottom ),
            "Returns a reference to the bottom node of the tree stack."
        )
        .def(
            "AssignRanks",
            ( void ( ::genesis::NewickBroker::* )(  ))( &::genesis::NewickBroker::AssignRanks ),
            "Iterate over the tree and assign ranks (= number of immediate children) to all nodes."
        )
        .def(
            "LeafCount",
            ( int ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::LeafCount ),
            "Returns the number of leaf nodes in the tree. AssignRanks() has to be called first."
        )
        .def(
            "InnerCount",
            ( int ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::InnerCount )
        )
        .def(
            "NodeCount",
            ( int ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::NodeCount )
        )
        .def(
            "MaxRank",
            ( int ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::MaxRank ),
            "Returns the highest rank of the nodes in the tree. AssignRanks() has to be called first."
        )
        .def(
            "IsBifurcating",
            ( bool ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::IsBifurcating )
        )
        .def(
            "Validate",
            ( bool ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::Validate ),
            "Returns true iff the tree is valid. AssignRanks() has to be called first."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::NewickBroker::* )(  ) const )( &::genesis::NewickBroker::Dump ),
            "Return a readable string representation of the elements of the NewickBroker."
        )

        // Operators

        .def(
            "__getitem__",
            ( NewickBrokerElement * ( ::genesis::NewickBroker::* )( const std::size_t ) const )( &::genesis::NewickBroker::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the nodes."
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::NewickBroker::begin, &::genesis::NewickBroker::end )
        )
        .add_property(
            "",
            boost::python::range ( &::genesis::NewickBroker::begin, &::genesis::NewickBroker::end )
        )
    ;

    // -------------------------------------------------------------------
    //     Class NewickLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::NewickLexer > ( "NewickLexer", boost::python::init<  >(  ) )
    ;

    // -------------------------------------------------------------------
    //     Class NewickProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::NewickProcessor > ( "NewickProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( * )( const std::string, Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string, Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::FromString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("FromString")
        .def(
            "FromLexer",
            ( bool ( * )( const NewickLexer &, Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::FromLexer ),
            ( boost::python::arg("lexer"), boost::python::arg("tree") )
        )
        .staticmethod("FromLexer")
        .def(
            "FromBroker",
            ( void ( * )( NewickBroker &, Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::FromBroker ),
            ( boost::python::arg("broker"), boost::python::arg("tree") )
        )
        .staticmethod("FromBroker")
        .def(
            "ToFile",
            ( bool ( * )( const std::string, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::ToString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::ToString ),
            ( boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToBroker",
            ( void ( * )( NewickBroker &, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::NewickProcessor::ToBroker ),
            ( boost::python::arg("broker"), boost::python::arg("tree") )
        )
        .staticmethod("ToBroker")
    ;

    // -------------------------------------------------------------------
    //     Class Options
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Options > ( "Options" )

        // Public Member Functions

        .def(
            "Init",
            ( void ( * )( int, char * ))( &::genesis::Options::Init ),
            ( boost::python::arg("argc"), boost::python::arg("argv") ),
            "Init method that takes the program's command line arguments."
        )
        .staticmethod("Init")
        .def(
            "GetCommandLine",
            ( std::vector< std::string > ( * )(  ))( &::genesis::Options::GetCommandLine ),
            "Returns an array of strings containing the program's command line arguments."
        )
        .staticmethod("GetCommandLine")
        .def(
            "GetCommandLineString",
            ( std::string ( * )(  ))( &::genesis::Options::GetCommandLineString ),
            "Returns a string containing the program's command line arguments."
        )
        .staticmethod("GetCommandLineString")
    ;

    // -------------------------------------------------------------------
    //     Class PhylipLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PhylipLexer > ( "PhylipLexer", boost::python::init<  >(  ) )
    ;

    // -------------------------------------------------------------------
    //     Class PhylipProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PhylipProcessor > ( "PhylipProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( * )( const std::string, SequenceSet & ))( &::genesis::PhylipProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, SequenceSet & ))( &::genesis::PhylipProcessor::FromString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( * )( const std::string, const SequenceSet & ))( &::genesis::PhylipProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const SequenceSet & ))( &::genesis::PhylipProcessor::ToString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const SequenceSet & ))( &::genesis::PhylipProcessor::ToString ),
            ( boost::python::arg("aln") )
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class PhyloXmlProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PhyloXmlProcessor > ( "PhyloXmlProcessor" )

        // Public Member Functions

        .def(
            "ToFile",
            ( bool ( * )( const std::string, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::PhyloXmlProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::PhyloXmlProcessor::ToString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::PhyloXmlProcessor::ToString ),
            ( boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToDocument",
            ( void ( * )( XmlDocument &, const Tree< NodeDataType, EdgeDataType > & ))( &::genesis::PhyloXmlProcessor::ToDocument ),
            ( boost::python::arg("xml"), boost::python::arg("tree") )
        )
        .staticmethod("ToDocument")
    ;

    // -------------------------------------------------------------------
    //     Class PlacementEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementEdgeData > ( "PlacementEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::PlacementEdgeData::* )( NewickBrokerElement * ))( &::genesis::PlacementEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::PlacementEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::PlacementEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlacementEdgeData::* )(  ) const )( &::genesis::PlacementEdgeData::Dump )
        )
        .def(
            "PlacementCount",
            ( size_t ( ::genesis::PlacementEdgeData::* )(  ) const )( &::genesis::PlacementEdgeData::PlacementCount ),
            "Returns the number of placements on this edge."
        )
        .def(
            "PlacementMass",
            ( double ( ::genesis::PlacementEdgeData::* )(  ) const )( &::genesis::PlacementEdgeData::PlacementMass ),
            "Returns the mass of the placements on this edge, as given by their like_weight_ratio."
        )
        .def(
            "SortPlacements",
            ( void ( ::genesis::PlacementEdgeData::* )(  ))( &::genesis::PlacementEdgeData::SortPlacements ),
            "Sorts the placements on this edge by their distal_length."
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementMap
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementMap > ( "PlacementMap", boost::python::init<  >(  ) )
        .def( boost::python::init< PlacementTree & >(( boost::python::arg("ptree") )) )
        .def( boost::python::init< const PlacementMap & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::clear ),
            "Clears all data of this object."
        )
        .def(
            "EdgeNumMap",
            ( EdgeNumMapType * ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::EdgeNumMap ),
            "Returns a mapping of edge_num integers to the corresponding Edge object."
        )
        .def(
            "Merge",
            ( bool ( ::genesis::PlacementMap::* )( const PlacementMap & ))( &::genesis::PlacementMap::Merge ),
            ( boost::python::arg("other") ),
            "Adds the pqueries from another PlacementMap objects to this one."
        )
        .def(
            "NormalizeWeightRatios",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::NormalizeWeightRatios ),
            "Recalculates the like_weight_ratio of the placements of each Pquery so that their sum is 1.0, while maintaining their ratio to each other."
        )
        .def(
            "RestrainToMaxWeightPlacements",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::RestrainToMaxWeightPlacements ),
            "Removes all placements but the most likely one from all pqueries."
        )
        .def(
            "PlacementCount",
            ( size_t ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::PlacementCount ),
            "Get the total number of placements in all pqueries."
        )
        .def(
            "PlacementMass",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::PlacementMass ),
            "Get the summed mass of all placements on the tree, given by their like_weight_ratio."
        )
        .def(
            "ClosestLeafDepthHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::ClosestLeafDepthHistogram ),
            "Returns a histogram representing how many placements have which depth with respect to their closest leaf node."
        )
        .def(
            "ClosestLeafDistanceHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( const double, const double, const int ) const )( &::genesis::PlacementMap::ClosestLeafDistanceHistogram ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns a histogram counting the number of placements that have a certain distance to their closest leaf node, divided into equally large intervals between a min and a max distance."
        )
        .def(
            "ClosestLeafDistanceHistogramAuto",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( double &, double &, const int ) const )( &::genesis::PlacementMap::ClosestLeafDistanceHistogramAuto ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns the same type of histogram as ClosestLeafDistanceHistogram(), but automatically determines the needed boundaries."
        )
        .def(
            "EMD",
            ( double ( ::genesis::PlacementMap::* )( const PlacementMap & ) const )( &::genesis::PlacementMap::EMD ),
            ( boost::python::arg("other") ),
            "Calculates the Earth Movers Distance to another sets of placements on a fixed reference tree."
        )
        .def(
            "COG",
            ( void ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::COG ),
            "Calculate the Center of Gravity of the placements on a tree."
        )
        .def(
            "Variance",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::Variance ),
            "Calculate the Variance of the placements on a tree."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::Dump ),
            "Returns a list of all Pqueries with their Placements and Names."
        )
        .def(
            "Validate",
            ( bool ( ::genesis::PlacementMap::* )( bool, bool ) const )( &::genesis::PlacementMap::Validate ),
            ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
            "Validates the integrity of the pointers, references and data in this Placement object."
        )
        .def(
            "EMD",
            ( double ( * )( const PlacementMap &, const PlacementMap & ))( &::genesis::PlacementMap::EMD ),
            ( boost::python::arg("left"), boost::python::arg("right") ),
            "Calculates the Earth Movers Distance between two sets of placements on a fixed reference tree."
        )
        .staticmethod("EMD")

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class PlacementNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementNodeData > ( "PlacementNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::PlacementNodeData::* )( NewickBrokerElement * ))( &::genesis::PlacementNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::PlacementNodeData::* )( NewickBrokerElement * ) const )( &::genesis::PlacementNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlacementNodeData::* )(  ) const )( &::genesis::PlacementNodeData::Dump )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementSimulator > ( "PlacementSimulator" )

        // Public Member Functions

        .def(
            "Generate",
            ( void ( * )( PlacementMap, size_t ))( &::genesis::PlacementSimulator::Generate ),
            ( boost::python::arg("placements"), boost::python::arg("n") ),
            "Generates n many Pqueries and places them in the PlacementMap."
        )
        .staticmethod("Generate")
        .def(
            "GenerateInSubtree",
            ( void ( * )( PlacementMap, size_t ))( &::genesis::PlacementSimulator::GenerateInSubtree ),
            ( boost::python::arg("placements"), boost::python::arg("n") )
        )
        .staticmethod("GenerateInSubtree")
    ;

    // -------------------------------------------------------------------
    //     Class Plausibility
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Plausibility > ( "Plausibility" )

        // Public Member Functions

        .def(
            "SpiderpigFunction",
            ( void ( ::genesis::Plausibility::* )( const std::string &, const std::string & ))( &::genesis::Plausibility::SpiderpigFunction ),
            ( boost::python::arg("reference_tree_file"), boost::python::arg("small_tree_file") )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlausibilityEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlausibilityEdgeData > ( "PlausibilityEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::PlausibilityEdgeData::* )( NewickBrokerElement * ))( &::genesis::PlausibilityEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::PlausibilityEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::PlausibilityEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlausibilityEdgeData::* )(  ) const )( &::genesis::PlausibilityEdgeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlausibilityNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlausibilityNodeData > ( "PlausibilityNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( ::genesis::PlausibilityNodeData::* )( NewickBrokerElement * ))( &::genesis::PlausibilityNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( ::genesis::PlausibilityNodeData::* )( NewickBrokerElement * ) const )( &::genesis::PlausibilityNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlausibilityNodeData::* )(  ) const )( &::genesis::PlausibilityNodeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Sequence
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Sequence > ( "Sequence", boost::python::init< std::string, std::string >(( boost::python::arg("label"), boost::python::arg("sites") )) )

        // Public Member Functions

        .def(
            "Label",
            ( std::string ( ::genesis::Sequence::* )(  ) const )( &::genesis::Sequence::Label )
        )
        .def(
            "Length",
            ( size_t ( ::genesis::Sequence::* )(  ) const )( &::genesis::Sequence::Length )
        )
        .def(
            "Site",
            ( SymbolType ( ::genesis::Sequence::* )( size_t ) const )( &::genesis::Sequence::Site ),
            ( boost::python::arg("index") )
        )
        .def(
            "Sites",
            ( const std::string & ( ::genesis::Sequence::* )(  ) const )( &::genesis::Sequence::Sites )
        )
        .def(
            "RemoveGaps",
            ( void ( ::genesis::Sequence::* )(  ))( &::genesis::Sequence::RemoveGaps ),
            "Removes all occurences of gap_char from the sequence."
        )
        .def(
            "Replace",
            ( void ( ::genesis::Sequence::* )( char, char ))( &::genesis::Sequence::Replace ),
            ( boost::python::arg("search"), boost::python::arg("replace") ),
            "Replaces all occurences of search by replace."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Sequence::* )(  ) const )( &::genesis::Sequence::Dump ),
            "Prints the label and the whole sequence (possibly very long!)."
        )
    ;

    // -------------------------------------------------------------------
    //     Class SequenceSet
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::SequenceSet > ( "SequenceSet" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::SequenceSet::* )(  ))( &::genesis::SequenceSet::clear ),
            "Deletes all sequences from the alignment."
        )
        .def(
            "FindSequence",
            ( Sequence * ( ::genesis::SequenceSet::* )( std::string ) const )( &::genesis::SequenceSet::FindSequence ),
            ( boost::python::arg("label") ),
            "Returns a pointer to a sequence with a specific label (or nullptr, if not found)."
        )
        .def(
            "RemoveList",
            ( void ( ::genesis::SequenceSet::* )( std::vector< std::string >, bool ))( &::genesis::SequenceSet::RemoveList ),
            ( boost::python::arg("labels"), boost::python::arg("invert")=(bool)(false) ),
            "Removes and deletes all those sequences from the Aligment whose labels are in the given list. If invert is set to true, it does the same inverted: it removes all except those in the list."
        )
        .def(
            "RemoveGaps",
            ( void ( ::genesis::SequenceSet::* )(  ))( &::genesis::SequenceSet::RemoveGaps ),
            "Calls RemoveGaps() for every Sequence."
        )
        .def(
            "Replace",
            ( void ( ::genesis::SequenceSet::* )( char, char ))( &::genesis::SequenceSet::Replace ),
            ( boost::python::arg("search"), boost::python::arg("replace") ),
            "Calls Replace() for every Sequence."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::SequenceSet::* )(  ) const )( &::genesis::SequenceSet::Dump ),
            "Gives a summary of the sequences names and their lengths for this alignment."
        )
    ;

    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::Tree > ( "Tree", boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeType & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::Tree::* )(  ))( &::genesis::Tree::clear )
        )
        .def(
            "swap",
            ( void ( ::genesis::Tree::* )( TreeType & ))( &::genesis::Tree::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "Import",
            ( void ( ::genesis::Tree::* )( LinkArray &, NodeArray &, EdgeArray & ))( &::genesis::Tree::Import ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        .def(
            "Export",
            ( void ( ::genesis::Tree::* )( LinkArray &, NodeArray &, EdgeArray & ))( &::genesis::Tree::Export ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        .def(
            "RootLink",
            ( LinkType * ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::RootLink )
        )
        .def(
            "RootNode",
            ( NodeType * ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::RootNode )
        )
        .def(
            "LinkAt",
            ( LinkType * ( ::genesis::Tree::* )( size_t ) const )( &::genesis::Tree::LinkAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "NodeAt",
            ( NodeType * ( ::genesis::Tree::* )( size_t ) const )( &::genesis::Tree::NodeAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "EdgeAt",
            ( EdgeType * ( ::genesis::Tree::* )( size_t ) const )( &::genesis::Tree::EdgeAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "LinkCount",
            ( size_t ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::LinkCount ),
            "Returns the number of Links of the Tree."
        )
        .def(
            "NodeCount",
            ( size_t ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::NodeCount ),
            "Returns the number of Nodes of the Tree."
        )
        .def(
            "EdgeCount",
            ( size_t ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::EdgeCount ),
            "Returns the number of Edges of the Tree."
        )
        .def(
            "FindNode",
            ( NodeType * ( ::genesis::Tree::* )( std::string ) const )( &::genesis::Tree::FindNode ),
            ( boost::python::arg("name") )
        )
        .def(
            "MaxRank",
            ( int ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::MaxRank )
        )
        .def(
            "IsBifurcating",
            ( bool ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::IsBifurcating )
        )
        .def(
            "LeafCount",
            ( size_t ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::LeafCount )
        )
        .def(
            "InnerCount",
            ( size_t ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::InnerCount )
        )
        .def(
            "Length",
            ( double ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::Length )
        )
        .def(
            "NodeDepthMatrix",
            ( Matrix< int > * ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::NodeDepthMatrix )
        )
        .def(
            "NodeDepthVector",
            ( std::vector< int > ( ::genesis::Tree::* )( const NodeType * ) const )( &::genesis::Tree::NodeDepthVector ),
            ( boost::python::arg("node")=(const NodeType *)(nullptr) )
        )
        .def(
            "NodeDistanceMatrix",
            ( Matrix< double > * ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::NodeDistanceMatrix )
        )
        .def(
            "NodeDistanceVector",
            ( std::vector< double > ( ::genesis::Tree::* )( const NodeType * ) const )( &::genesis::Tree::NodeDistanceVector ),
            ( boost::python::arg("node")=(const NodeType *)(nullptr) )
        )
        .def(
            "ClosestLeafDepthVector",
            ( NodeIntVectorType ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::ClosestLeafDepthVector )
        )
        .def(
            "ClosestLeafDistanceVector",
            ( NodeDoubleVectorType ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::ClosestLeafDistanceVector )
        )
        .def(
            "DeepestDistance",
            ( double ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::DeepestDistance )
        )
        .def(
            "Equal",
            ( bool ( ::genesis::Tree::* )( const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ) const )( &::genesis::Tree::Equal ),
            ( boost::python::arg("other"), boost::python::arg("comparator") )
        )
        .def(
            "HasIdenticalTopology",
            ( bool ( ::genesis::Tree::* )( const TreeType & ) const )( &::genesis::Tree::HasIdenticalTopology ),
            ( boost::python::arg("other") )
        )
        .def(
            "Validate",
            ( bool ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::Validate )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::Dump )
        )
        .def(
            "DumpLinks",
            ( std::string ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::DumpLinks )
        )
        .def(
            "DumpNodes",
            ( std::string ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::DumpNodes )
        )
        .def(
            "DumpEdges",
            ( std::string ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::DumpEdges )
        )
        .def(
            "DumpEulertour",
            ( std::string ( ::genesis::Tree::* )(  ) const )( &::genesis::Tree::DumpEulertour )
        )
        .def(
            "Equal",
            ( bool ( * )( const TreeType &, const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ))( &::genesis::Tree::Equal ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") )
        )
        .staticmethod("Equal")

        // Operators


        // Iterators

        .add_property(
            "Eulertour",
            boost::python::range ( &::genesis::Tree::BeginEulertour, &::genesis::Tree::EndEulertour )
        )
        .add_property(
            "Preorder",
            boost::python::range ( &::genesis::Tree::BeginPreorder, &::genesis::Tree::EndPreorder )
        )
        .add_property(
            "Postorder",
            boost::python::range ( &::genesis::Tree::BeginPostorder, &::genesis::Tree::EndPostorder )
        )
        .add_property(
            "Levelorder",
            boost::python::range ( &::genesis::Tree::BeginLevelorder, &::genesis::Tree::EndLevelorder )
        )
        .add_property(
            "Links",
            boost::python::range ( &::genesis::Tree::BeginLinks, &::genesis::Tree::EndLinks )
        )
        .add_property(
            "Nodes",
            boost::python::range ( &::genesis::Tree::BeginNodes, &::genesis::Tree::EndNodes )
        )
        .add_property(
            "Edges",
            boost::python::range ( &::genesis::Tree::BeginEdges, &::genesis::Tree::EndEdges )
        )
    ;

    // -------------------------------------------------------------------
    //     Class TreeEdge
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeEdge > ( "TreeEdge", boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeEdge & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "PrimaryLink",
            ( LinkType * ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::PrimaryLink ),
            "Returns the link of this edge that points towards the root."
        )
        .def(
            "SecondaryLink",
            ( LinkType * ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::SecondaryLink ),
            "Returns the link of this edge that points away from the root."
        )
        .def(
            "PrimaryNode",
            ( NodeType * ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::PrimaryNode )
        )
        .def(
            "SecondaryNode",
            ( NodeType * ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::SecondaryNode )
        )
        .def(
            "Index",
            ( size_t ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::Index ),
            "Returns the index of this Link."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::TreeEdge::* )(  ) const )( &::genesis::TreeEdge::Dump )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorEulertour
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeIteratorEulertour > ( "TreeIteratorEulertour", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "Link",
            ( LinkPointerType ( ::genesis::TreeIteratorEulertour::* )(  ) const )( &::genesis::TreeIteratorEulertour::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( ::genesis::TreeIteratorEulertour::* )(  ) const )( &::genesis::TreeIteratorEulertour::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( ::genesis::TreeIteratorEulertour::* )(  ) const )( &::genesis::TreeIteratorEulertour::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( ::genesis::TreeIteratorEulertour::* )(  ) const )( &::genesis::TreeIteratorEulertour::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( ::genesis::TreeIteratorEulertour::* )(  ) const )( &::genesis::TreeIteratorEulertour::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorLevelorder
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeIteratorLevelorder > ( "TreeIteratorLevelorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsFirstIteration",
            ( bool ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::IsFirstIteration )
        )
        .def(
            "Depth",
            ( int ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::Depth )
        )
        .def(
            "Link",
            ( LinkPointerType ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( ::genesis::TreeIteratorLevelorder::* )(  ) const )( &::genesis::TreeIteratorLevelorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorPostorder
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeIteratorPostorder > ( "TreeIteratorPostorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsLastIteration",
            ( bool ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::IsLastIteration )
        )
        .def(
            "Link",
            ( LinkPointerType ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( ::genesis::TreeIteratorPostorder::* )(  ) const )( &::genesis::TreeIteratorPostorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorPreorder
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeIteratorPreorder > ( "TreeIteratorPreorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsFirstIteration",
            ( bool ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::IsFirstIteration )
        )
        .def(
            "Link",
            ( LinkPointerType ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( ::genesis::TreeIteratorPreorder::* )(  ) const )( &::genesis::TreeIteratorPreorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeLink
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeLink > ( "TreeLink", boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeLink & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "Next",
            ( LinkType * ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Next ),
            "Returns a pointer to the next link within the node."
        )
        .def(
            "Outer",
            ( LinkType * ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Outer ),
            "Returns a pointer to the link of the adjacent node."
        )
        .def(
            "Edge",
            ( EdgeType * ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Edge ),
            "Returns a pointer to the TreeEdge containing the data of this link's edge."
        )
        .def(
            "Node",
            ( NodeType * ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Node ),
            "Returns a pointer to the TreeNode containing the data of this link's node."
        )
        .def(
            "Index",
            ( size_t ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Index ),
            "Returns the index of this Link."
        )
        .def(
            "IsLeaf",
            ( bool ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::IsLeaf ),
            "Returns true iff the node of this link is a leaf node."
        )
        .def(
            "IsInner",
            ( bool ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::IsInner ),
            "Returns true iff the node of this link is an inner node."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::TreeLink::* )(  ) const )( &::genesis::TreeLink::Dump ),
            "Returns a string containing dump information about this link."
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeNode
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeNode > ( "TreeNode", boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeNode & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "PrimaryLink",
            ( LinkType * ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::PrimaryLink ),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "Link",
            ( LinkType * ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::Link ),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "Index",
            ( size_t ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::Index ),
            "Returns the index of this Link."
        )
        .def(
            "Rank",
            ( int ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::Rank )
        )
        .def(
            "IsLeaf",
            ( bool ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::IsLeaf )
        )
        .def(
            "IsInner",
            ( bool ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::IsInner )
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::TreeNode::* )(  ) const )( &::genesis::TreeNode::Dump )
        )

        // Operators


        // Iterators

        .add_property(
            "Links",
            boost::python::range ( &::genesis::TreeNode::BeginLinks, &::genesis::TreeNode::EndLinks )
        )
    ;

    // -------------------------------------------------------------------
    //     Class TreeNodeIteratorLinks
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeNodeIteratorLinks > ( "TreeNodeIteratorLinks", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "Link",
            ( LinkPointerType ( ::genesis::TreeNodeIteratorLinks::* )(  ))( &::genesis::TreeNodeIteratorLinks::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( ::genesis::TreeNodeIteratorLinks::* )(  ))( &::genesis::TreeNodeIteratorLinks::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( ::genesis::TreeNodeIteratorLinks::* )(  ))( &::genesis::TreeNodeIteratorLinks::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( ::genesis::TreeNodeIteratorLinks::* )(  ))( &::genesis::TreeNodeIteratorLinks::StartLink )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeSet
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::TreeSet > ( "TreeSet", boost::python::init<  >(  ) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlComment
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlComment > ( "XmlComment", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlDocument > ( "XmlDocument" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::XmlDocument::* )(  ))( &::genesis::XmlDocument::clear )
        )
    ;

    // -------------------------------------------------------------------
    //     Class XmlElement
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlElement > ( "XmlElement", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::XmlElement::* )(  ))( &::genesis::XmlElement::clear )
        )
    ;

    // -------------------------------------------------------------------
    //     Class XmlMarkup
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlMarkup > ( "XmlMarkup", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlProcessor > ( "XmlProcessor" )

        // Public Member Functions

        .def(
            "ToFile",
            ( bool ( * )( const std::string &, const XmlDocument & ))( &::genesis::XmlProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Writes an XML file from an XmlDocument. Returns true iff successful."
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const XmlDocument & ))( &::genesis::XmlProcessor::ToString ),
            ( boost::python::arg("xml"), boost::python::arg("document") ),
            "Gives the XML string representation of a XmlDocument."
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const XmlDocument & ))( &::genesis::XmlProcessor::ToString ),
            ( boost::python::arg("document") ),
            "Returns the XML representation of a XmlDocument."
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class XmlValue
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::XmlValue > ( "XmlValue" )

        // Public Member Functions

        .def(
            "TypeToString",
            ( std::string ( * )( const Type ))( &::genesis::XmlValue::TypeToString ),
            ( boost::python::arg("t") )
        )
        .staticmethod("TypeToString")
        .def(
            "TypeToString",
            ( std::string ( ::genesis::XmlValue::* )(  ) const )( &::genesis::XmlValue::TypeToString )
        )
        .def(
            "type",
            ( Type ( ::genesis::XmlValue::* )(  ) const )( &::genesis::XmlValue::type )
        )
        .def(
            "IsComment",
            ( bool ( ::genesis::XmlValue::* )(  ))( &::genesis::XmlValue::IsComment )
        )
        .def(
            "IsMarkup",
            ( bool ( ::genesis::XmlValue::* )(  ))( &::genesis::XmlValue::IsMarkup )
        )
        .def(
            "IsElement",
            ( bool ( ::genesis::XmlValue::* )(  ))( &::genesis::XmlValue::IsElement )
        )
    ;


