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

    boost::python::class_< Bipartition > ( "Bipartition", boost::python::init< size_t >(( boost::python::arg("num_leaves") )) )

        // Public Member Functions

        .def(
            "Link",
            ( const LinkType * ( Bipartition::* )(  ))( &Bipartition::Link )
        )
        .def(
            "Invert",
            ( void ( Bipartition::* )(  ))( &Bipartition::Invert )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Bipartitions
    // -------------------------------------------------------------------

    boost::python::class_< Bipartitions > ( "Bipartitions", boost::python::init< const TreeType * >(( boost::python::arg("tree") )) )

        // Public Member Functions

        .def(
            "Make",
            ( void ( Bipartitions::* )(  ))( &Bipartitions::Make )
        )
        .def(
            "MakeIndex",
            ( void ( Bipartitions::* )(  ))( &Bipartitions::MakeIndex )
        )
        .def(
            "FindSmallestSubtree",
            ( BipartitionType * ( Bipartitions::* )( std::vector< NodeType * > ))( &Bipartitions::FindSmallestSubtree ),
            ( boost::python::arg("nodes") )
        )
        .def(
            "GetSubtreeEdges",
            ( std::vector< const EdgeType * > ( Bipartitions::* )( const LinkType * ))( &Bipartitions::GetSubtreeEdges ),
            ( boost::python::arg("subtree") )
        )
        .def(
            "Validate",
            ( bool ( Bipartitions::* )(  ))( &Bipartitions::Validate )
        )
        .def(
            "Dump",
            ( std::string ( Bipartitions::* )(  ))( &Bipartitions::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Bitvector
    // -------------------------------------------------------------------

    boost::python::class_< Bitvector > ( "Bitvector", boost::python::init< const size_t, boost::python::optional< const bool > >(( boost::python::arg("size"), boost::python::arg("init")=(const bool)(false) )) )

        // Public Member Functions

        .def(
            "size",
            ( size_t ( Bitvector::* )(  ) const )( &Bitvector::size ),
            "Returns the size (number of total bits) of this Bitvector."
        )
        .def(
            "Get",
            ( bool ( Bitvector::* )( size_t ) const )( &Bitvector::Get ),
            ( boost::python::arg("index") ),
            "Returns the value of a single bit, with boundary check."
        )
        .def(
            "Set",
            ( void ( Bitvector::* )( size_t ))( &Bitvector::Set ),
            ( boost::python::arg("index") ),
            "Sets the value of a single bit to true, with boundary check."
        )
        .def(
            "Unset",
            ( void ( Bitvector::* )( size_t ))( &Bitvector::Unset ),
            ( boost::python::arg("index") ),
            "Sets the value of a single bit to false, with boundary check."
        )
        .def(
            "Set",
            ( void ( Bitvector::* )( size_t, bool ))( &Bitvector::Set ),
            ( boost::python::arg("index"), boost::python::arg("value") ),
            "Sets the value of a single bit to a given bool value, with boundary check."
        )
        .def(
            "Flip",
            ( void ( Bitvector::* )( size_t ))( &Bitvector::Flip ),
            ( boost::python::arg("index") ),
            "Flips (inverts) the value of a single bit, with boundary check."
        )
        .def(
            "SymmetricDifference",
            ( Bitvector ( Bitvector::* )( Bitvector const & ) const )( &Bitvector::SymmetricDifference ),
            ( boost::python::arg("rhs") )
        )
        .def(
            "Count",
            ( size_t ( Bitvector::* )(  ) const )( &Bitvector::Count ),
            "Counts the number of set bits in the Bitvector."
        )
        .def(
            "Hash",
            ( size_t ( Bitvector::* )(  ) const )( &Bitvector::Hash ),
            "Returns an std::hash value for the Bitvector."
        )
        .def(
            "XHash",
            ( IntType ( Bitvector::* )(  ) const )( &Bitvector::XHash ),
            "Returns a hash value of type IntType, that is quicker to calculate than Hash(), and thus can be used where the std::hash is not needed."
        )
        .def(
            "Invert",
            ( void ( Bitvector::* )(  ))( &Bitvector::Invert ),
            "Flip all bits."
        )
        .def(
            "Normalize",
            ( void ( Bitvector::* )(  ))( &Bitvector::Normalize ),
            "Brings the Bitvector in a normalized form, where the first bit is always zero."
        )
        .def(
            "Reset",
            ( void ( Bitvector::* )( bool ))( &Bitvector::Reset ),
            ( boost::python::arg("value")=(bool)(false) ),
            "Reset all the bits to false. If provided with parameter true, sets all bits to true."
        )
        .def(
            "Dump",
            ( std::string ( Bitvector::* )(  ) const )( &Bitvector::Dump )
        )
        .def(
            "DumpInt",
            ( std::string ( Bitvector::* )( IntType ) const )( &Bitvector::DumpInt ),
            ( boost::python::arg("x") )
        )
        .def(
            "SymmetricDifference",
            ( Bitvector ( Bitvector::* )( Bitvector const &, Bitvector const & ))( &Bitvector::SymmetricDifference ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs") )
        )
        .staticmethod("SymmetricDifference")

        // Operators

        .def(
            "__getitem__",
            ( bool ( Bitvector::* )( size_t ) const )( &Bitvector::operator[] ),
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

    boost::python::class_< DefaultEdgeData > ( "DefaultEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( DefaultEdgeData::* )( NewickBrokerElement * ))( &DefaultEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") ),
            "Fills the edge with data from a NewickBrokerElement."
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( DefaultEdgeData::* )( NewickBrokerElement * ) const )( &DefaultEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( DefaultEdgeData::* )(  ) const )( &DefaultEdgeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class DefaultNodeData
    // -------------------------------------------------------------------

    boost::python::class_< DefaultNodeData > ( "DefaultNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( DefaultNodeData::* )( NewickBrokerElement * ))( &DefaultNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") ),
            "Fills the node with data from a NewickBrokerElement."
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( DefaultNodeData::* )( NewickBrokerElement * ) const )( &DefaultNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( DefaultNodeData::* )(  ) const )( &DefaultNodeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class FastaLexer
    // -------------------------------------------------------------------

    boost::python::class_< FastaLexer > ( "FastaLexer", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class FastaProcessor
    // -------------------------------------------------------------------

    boost::python::class_< FastaProcessor > ( "FastaProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( FastaProcessor::* )( const std::string, SequenceSet & ))( &FastaProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( FastaProcessor::* )( const std::string &, SequenceSet & ))( &FastaProcessor::FromString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( FastaProcessor::* )( const std::string, const SequenceSet & ))( &FastaProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( FastaProcessor::* )( std::string &, const SequenceSet & ))( &FastaProcessor::ToString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( FastaProcessor::* )( const SequenceSet & ))( &FastaProcessor::ToString ),
            ( boost::python::arg("aln") )
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class JplaceProcessor
    // -------------------------------------------------------------------

    boost::python::class_< JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "GetVersion",
            ( std::string ( JplaceProcessor::* )(  ))( &JplaceProcessor::GetVersion ),
            "Returns the version number that this class is written for."
        )
        .staticmethod("GetVersion")
        .def(
            "CheckVersion",
            ( bool ( JplaceProcessor::* )( const std::string ))( &JplaceProcessor::CheckVersion ),
            ( boost::python::arg("version") ),
            "Checks whether the version of the jplace format works with this parser."
        )
        .staticmethod("CheckVersion")
        .def(
            "FromFile",
            ( bool ( JplaceProcessor::* )( const std::string &, PlacementMap & ))( &JplaceProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( JplaceProcessor::* )( const std::string &, PlacementMap & ))( &JplaceProcessor::FromString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromString")
        .def(
            "FromDocument",
            ( bool ( JplaceProcessor::* )( const JsonDocument &, PlacementMap & ))( &JplaceProcessor::FromDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") ),
            "Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromDocument")
        .def(
            "ToFile",
            ( bool ( JplaceProcessor::* )( const std::string, const PlacementMap & ))( &JplaceProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( JplaceProcessor::* )( std::string &, const PlacementMap & ))( &JplaceProcessor::ToString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( JplaceProcessor::* )( const PlacementMap & ))( &JplaceProcessor::ToString ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToDocument",
            ( void ( JplaceProcessor::* )( JsonDocument &, const PlacementMap & ))( &JplaceProcessor::ToDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") )
        )
        .staticmethod("ToDocument")
    ;

    // -------------------------------------------------------------------
    //     Class JsonDocument
    // -------------------------------------------------------------------

    boost::python::class_< JsonDocument > ( "JsonDocument", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( JsonDocument::* )(  ) const )( &JsonDocument::ToString )
        )
        .def(
            "Validate",
            ( bool ( JsonDocument::* )(  ))( &JsonDocument::Validate )
        )
        .def(
            "Dump",
            ( std::string ( JsonDocument::* )(  ) const )( &JsonDocument::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonLexer
    // -------------------------------------------------------------------

    boost::python::class_< JsonLexer > ( "JsonLexer", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class JsonProcessor
    // -------------------------------------------------------------------

    boost::python::class_< JsonProcessor > ( "JsonProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( JsonProcessor::* )( const std::string &, JsonDocument & ))( &JsonProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Takes a JSON document file path and parses its contents into a JsonDocument."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( JsonProcessor::* )( const std::string &, JsonDocument & ))( &JsonProcessor::FromString ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            "Takes a string containing a JSON document and parses its contents into a JsonDocument."
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( JsonProcessor::* )( const std::string &, const JsonDocument & ))( &JsonProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Writes a Json file from a JsonDocument. Returns true iff successful."
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( JsonProcessor::* )( std::string &, const JsonDocument & ))( &JsonProcessor::ToString ),
            ( boost::python::arg("json"), boost::python::arg("document") ),
            "Gives the Json string representation of a JsonDocument."
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( JsonProcessor::* )( const JsonDocument & ))( &JsonProcessor::ToString ),
            ( boost::python::arg("document") ),
            "Returns the Json representation of a JsonDocument."
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class JsonValue
    // -------------------------------------------------------------------

    boost::python::class_< JsonValue > ( "JsonValue" )

        // Public Member Functions

        .def(
            "TypeToString",
            ( std::string ( JsonValue::* )( const Type ))( &JsonValue::TypeToString ),
            ( boost::python::arg("t") )
        )
        .staticmethod("TypeToString")
        .def(
            "TypeToString",
            ( std::string ( JsonValue::* )(  ) const )( &JsonValue::TypeToString )
        )
        .def(
            "type",
            ( Type ( JsonValue::* )(  ) const )( &JsonValue::type )
        )
        .def(
            "IsNull",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsNull )
        )
        .def(
            "IsBool",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsBool )
        )
        .def(
            "IsNumber",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsNumber )
        )
        .def(
            "IsString",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsString )
        )
        .def(
            "IsArray",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsArray )
        )
        .def(
            "IsObject",
            ( bool ( JsonValue::* )(  ))( &JsonValue::IsObject )
        )
        .def(
            "ToString",
            ( std::string ( JsonValue::* )(  ) const )( &JsonValue::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueArray
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueArray > ( "JsonValueArray", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "begin",
            ( iterator ( JsonValueArray::* )(  ))( &JsonValueArray::begin ),
            "Returns an iterator to the beginning of the array."
        )
        .def(
            "cbegin",
            ( const_iterator ( JsonValueArray::* )(  ) const )( &JsonValueArray::cbegin ),
            "Const version of begin()."
        )
        .def(
            "end",
            ( iterator ( JsonValueArray::* )(  ))( &JsonValueArray::end ),
            "Returns an iterator to the end of the array."
        )
        .def(
            "cend",
            ( const_iterator ( JsonValueArray::* )(  ) const )( &JsonValueArray::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( JsonValue * ( JsonValueArray::* )( const std::size_t ) const )( &JsonValueArray::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the array, doing a boundary check first."
        )
        .def(
            "empty",
            ( bool ( JsonValueArray::* )(  ) const )( &JsonValueArray::empty ),
            "Returns whether the array is empty."
        )
        .def(
            "size",
            ( size_t ( JsonValueArray::* )(  ) const )( &JsonValueArray::size ),
            "Returns the number of values in the array."
        )
        .def(
            "push_back",
            ( void ( JsonValueArray::* )( JsonValue * ))( &JsonValueArray::push_back ),
            ( boost::python::arg("value") )
        )
        .def(
            "Add",
            ( void ( JsonValueArray::* )( JsonValue * ))( &JsonValueArray::Add ),
            ( boost::python::arg("value") ),
            "Alias of push_back()."
        )
        .def(
            "pop_back",
            ( void ( JsonValueArray::* )(  ))( &JsonValueArray::pop_back )
        )
        .def(
            "clear",
            ( void ( JsonValueArray::* )(  ))( &JsonValueArray::clear ),
            "Clears all values, as if the array was newly created."
        )
        .def(
            "ToString",
            ( std::string ( JsonValueArray::* )(  ) const )( &JsonValueArray::ToString )
        )

        // Operators

        .def(
            "__getitem__",
            ( JsonValue * ( JsonValueArray::* )( const std::size_t ) const )( &JsonValueArray::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the array."
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueBool
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueBool > ( "JsonValueBool", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( JsonValueBool::* )(  ) const )( &JsonValueBool::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueNull
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueNull > ( "JsonValueNull", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( JsonValueNull::* )(  ) const )( &JsonValueNull::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueNumber
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueNumber > ( "JsonValueNumber", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( JsonValueNumber::* )(  ) const )( &JsonValueNumber::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueObject
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueObject > ( "JsonValueObject", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "begin",
            ( iterator ( JsonValueObject::* )(  ))( &JsonValueObject::begin ),
            "Returns an iterator to the beginning of the object."
        )
        .def(
            "cbegin",
            ( const_iterator ( JsonValueObject::* )(  ) const )( &JsonValueObject::cbegin ),
            "Const version of begin()."
        )
        .def(
            "end",
            ( iterator ( JsonValueObject::* )(  ))( &JsonValueObject::end ),
            "Returns an iterator to the end of the object."
        )
        .def(
            "cend",
            ( const_iterator ( JsonValueObject::* )(  ) const )( &JsonValueObject::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( JsonValue * ( JsonValueObject::* )( const std::string & ) const )( &JsonValueObject::at ),
            ( boost::python::arg("name") ),
            "Provides index based array access to the object, doing a boundary check first. This is an alias for Get()."
        )
        .def(
            "empty",
            ( bool ( JsonValueObject::* )(  ) const )( &JsonValueObject::empty ),
            "Returns whether the object is empty."
        )
        .def(
            "size",
            ( size_t ( JsonValueObject::* )(  ) const )( &JsonValueObject::size ),
            "Returns the number of values in the object."
        )
        .def(
            "Has",
            ( bool ( JsonValueObject::* )( const std::string & ) const )( &JsonValueObject::Has ),
            ( boost::python::arg("name") ),
            "Returns true iff the object contains a certain key."
        )
        .def(
            "Get",
            ( JsonValue * ( JsonValueObject::* )( const std::string & ) const )( &JsonValueObject::Get ),
            ( boost::python::arg("name") ),
            "Returns the value of a certain key if present in the object, nullptr otherwise."
        )
        .def(
            "clear",
            ( void ( JsonValueObject::* )(  ))( &JsonValueObject::clear ),
            "Clears all values, as if the object was newly created."
        )
        .def(
            "erase",
            ( bool ( JsonValueObject::* )( const std::string ))( &JsonValueObject::erase ),
            ( boost::python::arg("name") ),
            "Clears the value of a specific key; returns true iff this key existed."
        )
        .def(
            "Set",
            ( void ( JsonValueObject::* )( const std::string &, JsonValue * ))( &JsonValueObject::Set ),
            ( boost::python::arg("name"), boost::python::arg("value") ),
            "Sets the value for a certain key."
        )
        .def(
            "ToString",
            ( std::string ( JsonValueObject::* )(  ) const )( &JsonValueObject::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class JsonValueString
    // -------------------------------------------------------------------

    boost::python::class_< JsonValueString > ( "JsonValueString", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "ToString",
            ( std::string ( JsonValueString::* )(  ) const )( &JsonValueString::ToString )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Lexer
    // -------------------------------------------------------------------

    boost::python::class_< Lexer > ( "Lexer" )

        // Public Member Functions

        .def(
            "ProcessFile",
            ( bool ( Lexer::* )( const std::string & ))( &Lexer::ProcessFile ),
            ( boost::python::arg("fn") ),
            "Shortcut function that reads the contents of a file and then calls ProcessString()."
        )
        .def(
            "ProcessString",
            ( bool ( Lexer::* )( const std::string &, bool ))( &Lexer::ProcessString ),
            ( boost::python::arg("text"), boost::python::arg("stepwise")=(bool)(false) ),
            "Process a string and store the resulting tokens in this Lexer object."
        )
        .def(
            "ProcessStep",
            ( bool ( Lexer::* )(  ))( &Lexer::ProcessStep ),
            "Processes one step of the lexing."
        )
        .def(
            "ValidateBrackets",
            ( bool ( Lexer::* )(  ) const )( &Lexer::ValidateBrackets ),
            "Checkes whether the bracket tokes are validly nested."
        )
        .def(
            "Dump",
            ( std::string ( Lexer::* )(  ) const )( &Lexer::Dump ),
            "Returns a listing of the parse result in readable form."
        )
        .def(
            "begin",
            ( iterator ( Lexer::* )(  ))( &Lexer::begin ),
            "Returns an iterator to the beginning of the token list."
        )
        .def(
            "end",
            ( iterator ( Lexer::* )(  ))( &Lexer::end ),
            "Returns an iterator to the end of the token list."
        )
        .def(
            "cbegin",
            ( const_iterator ( Lexer::* )(  ) const )( &Lexer::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( Lexer::* )(  ) const )( &Lexer::cend ),
            "Const version of end()."
        )
        .def(
            "at",
            ( LexerToken ( Lexer::* )( const std::size_t ) const )( &Lexer::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the tokens, doing a boundary check first."
        )
        .def(
            "front",
            ( LexerToken ( Lexer::* )(  ) const )( &Lexer::front ),
            "Returns a reference to the first token."
        )
        .def(
            "back",
            ( LexerToken ( Lexer::* )(  ) const )( &Lexer::back ),
            "Returns a reference to the last token."
        )
        .def(
            "empty",
            ( bool ( Lexer::* )(  ) const )( &Lexer::empty ),
            "Returns whether the list of tokens is empty."
        )
        .def(
            "size",
            ( size_t ( Lexer::* )(  ) const )( &Lexer::size ),
            "Returns the number of tokens produced during the analysis process."
        )
        .def(
            "clear",
            ( void ( Lexer::* )(  ))( &Lexer::clear ),
            "Clears all tokens, as if the object was newly created."
        )
        .def(
            "HasError",
            ( bool ( Lexer::* )(  ) const )( &Lexer::HasError ),
            "Returns whether there appeared an error while lexing."
        )

        // Operators

        .def(
            "__getitem__",
            ( LexerToken ( Lexer::* )( const std::size_t ) const )( &Lexer::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the tokens."
        )
    ;

    // -------------------------------------------------------------------
    //     Class LexerIterator
    // -------------------------------------------------------------------

    boost::python::class_< LexerIterator > ( "LexerIterator", boost::python::init< Lexer &, int >(( boost::python::arg("lexer"), boost::python::arg("position") )) )

        // Public Member Functions

        .def(
            "ConsumeWithTail",
            ( void ( LexerIterator::* )( const int ))( &LexerIterator::ConsumeWithTail ),
            ( boost::python::arg("tail_size") ),
            "Determines the consumption policy of the iterator when traversion the Lexer."
        )
        .def(
            "ProduceWithHead",
            ( void ( LexerIterator::* )( const int ))( &LexerIterator::ProduceWithHead ),
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

    boost::python::class_< Logging > ( "Logging", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "Get",
            ( std::ostringstream & ( Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel ))( &Logging::Get ),
            ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level") ),
            "Getter for the singleton instance of log, is called by the standard macros."
        )
        .def(
            "Get",
            ( std::ostringstream & ( Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel, const LoggingDetails ))( &Logging::Get ),
            ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level"), boost::python::arg("dets") ),
            "Getter for the singleton instance of log, is called by special macros that change the details of the log message."
        )
        .def(
            "AddOutputStream",
            ( void ( Logging::* )( std::ostream & ))( &Logging::AddOutputStream ),
            ( boost::python::arg("os") ),
            "Add an output stream to which log messages are written."
        )
        .staticmethod("AddOutputStream")
        .def(
            "AddOutputFile",
            ( void ( Logging::* )( const std::string & ))( &Logging::AddOutputFile ),
            ( boost::python::arg("fn") ),
            "Add an output file to which log messages are written."
        )
        .staticmethod("AddOutputFile")
        .def(
            "max_level",
            ( LoggingLevel ( Logging::* )(  ))( &Logging::max_level ),
            "Get the highest log level that is reported."
        )
        .staticmethod("max_level")
        .def(
            "max_level",
            ( void ( Logging::* )( const LoggingLevel ))( &Logging::max_level ),
            ( boost::python::arg("level") ),
            "Set the highest log level that is reported."
        )
        .staticmethod("max_level")
        .def(
            "report_percentage",
            ( int ( Logging::* )(  ))( &Logging::report_percentage ),
            "Get the current percentage for reporting LOG_PROG messages."
        )
        .staticmethod("report_percentage")
        .def(
            "report_percentage",
            ( void ( Logging::* )( const int ))( &Logging::report_percentage ),
            ( boost::python::arg("percentage") ),
            "set the percentage for reporting LOG_PROG messages."
        )
        .staticmethod("report_percentage")
        .def(
            "LevelToString",
            ( std::string ( Logging::* )( const LoggingLevel ))( &Logging::LevelToString ),
            ( boost::python::arg("level") ),
            "Return a string representation of a log level."
        )
        .staticmethod("LevelToString")

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class Matrix
    // -------------------------------------------------------------------

    boost::python::class_< Matrix > ( "Matrix", boost::python::init< size_t, size_t >(( boost::python::arg("rows"), boost::python::arg("cols") )) )

        // Public Member Functions

        .def(
            "Rows",
            ( size_t ( Matrix::* )(  ) const )( &Matrix::Rows )
        )
        .def(
            "Cols",
            ( size_t ( Matrix::* )(  ) const )( &Matrix::Cols )
        )
        .def(
            "size",
            ( size_t ( Matrix::* )(  ) const )( &Matrix::size )
        )
        .def(
            "Dump",
            ( std::string ( Matrix::* )(  ))( &Matrix::Dump )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class NewickBroker
    // -------------------------------------------------------------------

    boost::python::class_< NewickBroker > ( "NewickBroker" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( NewickBroker::* )(  ))( &NewickBroker::clear ),
            "Deletes all nodes from the broker."
        )
        .def(
            "PushTop",
            ( void ( NewickBroker::* )( NewickBrokerElement * ))( &NewickBroker::PushTop ),
            ( boost::python::arg("node") )
        )
        .def(
            "PushBottom",
            ( void ( NewickBroker::* )( NewickBrokerElement * ))( &NewickBroker::PushBottom ),
            ( boost::python::arg("node") )
        )
        .def(
            "PopTop",
            ( void ( NewickBroker::* )(  ))( &NewickBroker::PopTop )
        )
        .def(
            "PopBottom",
            ( void ( NewickBroker::* )(  ))( &NewickBroker::PopBottom )
        )
        .def(
            "begin",
            ( iterator ( NewickBroker::* )(  ))( &NewickBroker::begin ),
            "Returns an iterator to the top of the stack."
        )
        .def(
            "end",
            ( iterator ( NewickBroker::* )(  ))( &NewickBroker::end ),
            "Returns an iterator to the end of the token list."
        )
        .def(
            "cbegin",
            ( const_iterator ( NewickBroker::* )(  ) const )( &NewickBroker::cbegin ),
            "Const version of begin()."
        )
        .def(
            "cend",
            ( const_iterator ( NewickBroker::* )(  ) const )( &NewickBroker::cend ),
            "Const version of end()."
        )
        .def(
            "rbegin",
            ( reverse_iterator ( NewickBroker::* )(  ))( &NewickBroker::rbegin ),
            "Returns a reverse iterator to the nodes on the stack."
        )
        .def(
            "rend",
            ( reverse_iterator ( NewickBroker::* )(  ))( &NewickBroker::rend ),
            "Reverse version of end()."
        )
        .def(
            "crbegin",
            ( const_reverse_iterator ( NewickBroker::* )(  ))( &NewickBroker::crbegin ),
            "Const version of rbegin()."
        )
        .def(
            "crend",
            ( const_reverse_iterator ( NewickBroker::* )(  ))( &NewickBroker::crend ),
            "Const version of rend()."
        )
        .def(
            "empty",
            ( bool ( NewickBroker::* )(  ) const )( &NewickBroker::empty ),
            "Returns whether the stack is empty."
        )
        .def(
            "size",
            ( size_t ( NewickBroker::* )(  ) const )( &NewickBroker::size ),
            "Returns the size of the stack, i.e. the number of nodes stored in the broker."
        )
        .def(
            "at",
            ( NewickBrokerElement * ( NewickBroker::* )( const std::size_t ) const )( &NewickBroker::at ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the nodes, doing a boundary check first."
        )
        .def(
            "Top",
            ( NewickBrokerElement * ( NewickBroker::* )(  ))( &NewickBroker::Top ),
            "Returns a reference to the top node of the tree stack."
        )
        .def(
            "Bottom",
            ( NewickBrokerElement * ( NewickBroker::* )(  ))( &NewickBroker::Bottom ),
            "Returns a reference to the bottom node of the tree stack."
        )
        .def(
            "AssignRanks",
            ( void ( NewickBroker::* )(  ))( &NewickBroker::AssignRanks ),
            "Iterate over the tree and assign ranks (= number of immediate children) to all nodes."
        )
        .def(
            "LeafCount",
            ( int ( NewickBroker::* )(  ) const )( &NewickBroker::LeafCount ),
            "Returns the number of leaf nodes in the tree. AssignRanks() has to be called first."
        )
        .def(
            "InnerCount",
            ( int ( NewickBroker::* )(  ) const )( &NewickBroker::InnerCount )
        )
        .def(
            "NodeCount",
            ( int ( NewickBroker::* )(  ) const )( &NewickBroker::NodeCount )
        )
        .def(
            "MaxRank",
            ( int ( NewickBroker::* )(  ) const )( &NewickBroker::MaxRank ),
            "Returns the highest rank of the nodes in the tree. AssignRanks() has to be called first."
        )
        .def(
            "IsBifurcating",
            ( bool ( NewickBroker::* )(  ) const )( &NewickBroker::IsBifurcating )
        )
        .def(
            "Validate",
            ( bool ( NewickBroker::* )(  ) const )( &NewickBroker::Validate ),
            "Returns true iff the tree is valid. AssignRanks() has to be called first."
        )
        .def(
            "Dump",
            ( std::string ( NewickBroker::* )(  ) const )( &NewickBroker::Dump ),
            "Return a readable string representation of the elements of the NewickBroker."
        )

        // Operators

        .def(
            "__getitem__",
            ( NewickBrokerElement * ( NewickBroker::* )( const std::size_t ) const )( &NewickBroker::operator[] ),
            ( boost::python::arg("index") ),
            "Provides index based array access to the nodes."
        )
    ;

    // -------------------------------------------------------------------
    //     Class NewickLexer
    // -------------------------------------------------------------------

    boost::python::class_< NewickLexer > ( "NewickLexer", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class NewickProcessor
    // -------------------------------------------------------------------

    boost::python::class_< NewickProcessor > ( "NewickProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( NewickProcessor::* )( const std::string, Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( NewickProcessor::* )( const std::string, Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::FromString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("FromString")
        .def(
            "FromLexer",
            ( bool ( NewickProcessor::* )( const NewickLexer &, Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::FromLexer ),
            ( boost::python::arg("lexer"), boost::python::arg("tree") )
        )
        .staticmethod("FromLexer")
        .def(
            "FromBroker",
            ( void ( NewickProcessor::* )( NewickBroker &, Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::FromBroker ),
            ( boost::python::arg("broker"), boost::python::arg("tree") )
        )
        .staticmethod("FromBroker")
        .def(
            "ToFile",
            ( bool ( NewickProcessor::* )( const std::string, const Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( NewickProcessor::* )( std::string &, const Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::ToString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( NewickProcessor::* )( const Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::ToString ),
            ( boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToBroker",
            ( void ( NewickProcessor::* )( NewickBroker &, const Tree< NodeDataType, EdgeDataType > & ))( &NewickProcessor::ToBroker ),
            ( boost::python::arg("broker"), boost::python::arg("tree") )
        )
        .staticmethod("ToBroker")
    ;

    // -------------------------------------------------------------------
    //     Class Options
    // -------------------------------------------------------------------

    boost::python::class_< Options > ( "Options" )

        // Public Member Functions

        .def(
            "Init",
            ( void ( Options::* )( int, char * ))( &Options::Init ),
            ( boost::python::arg("argc"), boost::python::arg("argv") ),
            "Init method that takes the program's command line arguments."
        )
        .staticmethod("Init")
        .def(
            "GetCommandLine",
            ( std::vector< std::string > ( Options::* )(  ))( &Options::GetCommandLine ),
            "Returns an array of strings containing the program's command line arguments."
        )
        .staticmethod("GetCommandLine")
        .def(
            "GetCommandLineString",
            ( std::string ( Options::* )(  ))( &Options::GetCommandLineString ),
            "Returns a string containing the program's command line arguments."
        )
        .staticmethod("GetCommandLineString")
    ;

    // -------------------------------------------------------------------
    //     Class PhylipLexer
    // -------------------------------------------------------------------

    boost::python::class_< PhylipLexer > ( "PhylipLexer", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class PhylipProcessor
    // -------------------------------------------------------------------

    boost::python::class_< PhylipProcessor > ( "PhylipProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( PhylipProcessor::* )( const std::string, SequenceSet & ))( &PhylipProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( PhylipProcessor::* )( const std::string &, SequenceSet & ))( &PhylipProcessor::FromString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("FromString")
        .def(
            "ToFile",
            ( bool ( PhylipProcessor::* )( const std::string, const SequenceSet & ))( &PhylipProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("aln") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( PhylipProcessor::* )( std::string &, const SequenceSet & ))( &PhylipProcessor::ToString ),
            ( boost::python::arg("fs"), boost::python::arg("aln") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( PhylipProcessor::* )( const SequenceSet & ))( &PhylipProcessor::ToString ),
            ( boost::python::arg("aln") )
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class PhyloXmlProcessor
    // -------------------------------------------------------------------

    boost::python::class_< PhyloXmlProcessor > ( "PhyloXmlProcessor" )

        // Public Member Functions

        .def(
            "ToFile",
            ( bool ( PhyloXmlProcessor::* )( const std::string, const Tree< NodeDataType, EdgeDataType > & ))( &PhyloXmlProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( PhyloXmlProcessor::* )( std::string &, const Tree< NodeDataType, EdgeDataType > & ))( &PhyloXmlProcessor::ToString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( PhyloXmlProcessor::* )( const Tree< NodeDataType, EdgeDataType > & ))( &PhyloXmlProcessor::ToString ),
            ( boost::python::arg("tree") )
        )
        .staticmethod("ToString")
        .def(
            "ToDocument",
            ( void ( PhyloXmlProcessor::* )( XmlDocument &, const Tree< NodeDataType, EdgeDataType > & ))( &PhyloXmlProcessor::ToDocument ),
            ( boost::python::arg("xml"), boost::python::arg("tree") )
        )
        .staticmethod("ToDocument")
    ;

    // -------------------------------------------------------------------
    //     Class PlacementEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< PlacementEdgeData > ( "PlacementEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( PlacementEdgeData::* )( NewickBrokerElement * ))( &PlacementEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( PlacementEdgeData::* )( NewickBrokerElement * ) const )( &PlacementEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "Dump",
            ( std::string ( PlacementEdgeData::* )(  ) const )( &PlacementEdgeData::Dump )
        )
        .def(
            "PlacementCount",
            ( size_t ( PlacementEdgeData::* )(  ) const )( &PlacementEdgeData::PlacementCount ),
            "Returns the number of placements on this edge."
        )
        .def(
            "PlacementMass",
            ( double ( PlacementEdgeData::* )(  ) const )( &PlacementEdgeData::PlacementMass ),
            "Returns the mass of the placements on this edge, as given by their like_weight_ratio."
        )
        .def(
            "SortPlacements",
            ( void ( PlacementEdgeData::* )(  ))( &PlacementEdgeData::SortPlacements ),
            "Sorts the placements on this edge by their distal_length."
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementMap
    // -------------------------------------------------------------------

    boost::python::class_< PlacementMap > ( "PlacementMap", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( PlacementMap::* )(  ))( &PlacementMap::clear ),
            "Clears all data of this object."
        )
        .def(
            "EdgeNumMap",
            ( EdgeNumMapType * ( PlacementMap::* )(  ) const )( &PlacementMap::EdgeNumMap ),
            "Returns a mapping of edge_num integers to the corresponding Edge object."
        )
        .def(
            "Merge",
            ( bool ( PlacementMap::* )( const PlacementMap & ))( &PlacementMap::Merge ),
            ( boost::python::arg("other") ),
            "Adds the pqueries from another PlacementMap objects to this one."
        )
        .def(
            "NormalizeWeightRatios",
            ( void ( PlacementMap::* )(  ))( &PlacementMap::NormalizeWeightRatios ),
            "Recalculates the like_weight_ratio of the placements of each Pquery so that their sum is 1.0, while maintaining their ratio to each other."
        )
        .def(
            "RestrainToMaxWeightPlacements",
            ( void ( PlacementMap::* )(  ))( &PlacementMap::RestrainToMaxWeightPlacements ),
            "Removes all placements but the most likely one from all pqueries."
        )
        .def(
            "PlacementCount",
            ( size_t ( PlacementMap::* )(  ) const )( &PlacementMap::PlacementCount ),
            "Get the total number of placements in all pqueries."
        )
        .def(
            "PlacementMass",
            ( double ( PlacementMap::* )(  ) const )( &PlacementMap::PlacementMass ),
            "Get the summed mass of all placements on the tree, given by their like_weight_ratio."
        )
        .def(
            "ClosestLeafDepthHistogram",
            ( std::vector< int > ( PlacementMap::* )(  ) const )( &PlacementMap::ClosestLeafDepthHistogram ),
            "Returns a histogram representing how many placements have which depth with respect to their closest leaf node."
        )
        .def(
            "ClosestLeafDistanceHistogram",
            ( std::vector< int > ( PlacementMap::* )( const double, const double, const int ) const )( &PlacementMap::ClosestLeafDistanceHistogram ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns a histogram counting the number of placements that have a certain distance to their closest leaf node, divided into equally large intervals between a min and a max distance."
        )
        .def(
            "ClosestLeafDistanceHistogramAuto",
            ( std::vector< int > ( PlacementMap::* )( double &, double &, const int ) const )( &PlacementMap::ClosestLeafDistanceHistogramAuto ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns the same type of histogram as ClosestLeafDistanceHistogram(), but automatically determines the needed boundaries."
        )
        .def(
            "EMD",
            ( double ( PlacementMap::* )( const PlacementMap & ) const )( &PlacementMap::EMD ),
            ( boost::python::arg("other") ),
            "Calculates the Earth Movers Distance to another sets of placements on a fixed reference tree."
        )
        .def(
            "COG",
            ( void ( PlacementMap::* )(  ) const )( &PlacementMap::COG ),
            "Calculate the Center of Gravity of the placements on a tree."
        )
        .def(
            "Variance",
            ( double ( PlacementMap::* )(  ) const )( &PlacementMap::Variance ),
            "Calculate the Variance of the placements on a tree."
        )
        .def(
            "Dump",
            ( std::string ( PlacementMap::* )(  ) const )( &PlacementMap::Dump ),
            "Returns a list of all Pqueries with their Placements and Names."
        )
        .def(
            "Validate",
            ( bool ( PlacementMap::* )( bool, bool ) const )( &PlacementMap::Validate ),
            ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
            "Validates the integrity of the pointers, references and data in this Placement object."
        )
        .def(
            "EMD",
            ( double ( PlacementMap::* )( const PlacementMap &, const PlacementMap & ))( &PlacementMap::EMD ),
            ( boost::python::arg("left"), boost::python::arg("right") ),
            "Calculates the Earth Movers Distance between two sets of placements on a fixed reference tree."
        )
        .staticmethod("EMD")

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class PlacementNodeData
    // -------------------------------------------------------------------

    boost::python::class_< PlacementNodeData > ( "PlacementNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( PlacementNodeData::* )( NewickBrokerElement * ))( &PlacementNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( PlacementNodeData::* )( NewickBrokerElement * ) const )( &PlacementNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( PlacementNodeData::* )(  ) const )( &PlacementNodeData::Dump )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class PlacementSimulator
    // -------------------------------------------------------------------

    boost::python::class_< PlacementSimulator > ( "PlacementSimulator" )

        // Public Member Functions

        .def(
            "Generate",
            ( void ( PlacementSimulator::* )( PlacementMap, size_t ))( &PlacementSimulator::Generate ),
            ( boost::python::arg("placements"), boost::python::arg("n") ),
            "Generates n many Pqueries and places them in the PlacementMap."
        )
        .staticmethod("Generate")
        .def(
            "GenerateInSubtree",
            ( void ( PlacementSimulator::* )( PlacementMap, size_t ))( &PlacementSimulator::GenerateInSubtree ),
            ( boost::python::arg("placements"), boost::python::arg("n") )
        )
        .staticmethod("GenerateInSubtree")
    ;

    // -------------------------------------------------------------------
    //     Class Plausibility
    // -------------------------------------------------------------------

    boost::python::class_< Plausibility > ( "Plausibility" )

        // Public Member Functions

        .def(
            "SpiderpigFunction",
            ( void ( Plausibility::* )( const std::string &, const std::string & ))( &Plausibility::SpiderpigFunction ),
            ( boost::python::arg("reference_tree_file"), boost::python::arg("small_tree_file") )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlausibilityEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< PlausibilityEdgeData > ( "PlausibilityEdgeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( PlausibilityEdgeData::* )( NewickBrokerElement * ))( &PlausibilityEdgeData::FromNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( PlausibilityEdgeData::* )( NewickBrokerElement * ) const )( &PlausibilityEdgeData::ToNewickBrokerElement ),
            ( boost::python::arg("nbe") )
        )
        .def(
            "Dump",
            ( std::string ( PlausibilityEdgeData::* )(  ) const )( &PlausibilityEdgeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class PlausibilityNodeData
    // -------------------------------------------------------------------

    boost::python::class_< PlausibilityNodeData > ( "PlausibilityNodeData" )

        // Public Member Functions

        .def(
            "FromNewickBrokerElement",
            ( void ( PlausibilityNodeData::* )( NewickBrokerElement * ))( &PlausibilityNodeData::FromNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "ToNewickBrokerElement",
            ( void ( PlausibilityNodeData::* )( NewickBrokerElement * ) const )( &PlausibilityNodeData::ToNewickBrokerElement ),
            ( boost::python::arg("node") )
        )
        .def(
            "Dump",
            ( std::string ( PlausibilityNodeData::* )(  ) const )( &PlausibilityNodeData::Dump )
        )
    ;

    // -------------------------------------------------------------------
    //     Class Sequence
    // -------------------------------------------------------------------

    boost::python::class_< Sequence > ( "Sequence", boost::python::init< std::string, std::string >(( boost::python::arg("label"), boost::python::arg("sites") )) )

        // Public Member Functions

        .def(
            "Label",
            ( std::string ( Sequence::* )(  ) const )( &Sequence::Label )
        )
        .def(
            "Length",
            ( size_t ( Sequence::* )(  ) const )( &Sequence::Length )
        )
        .def(
            "Site",
            ( SymbolType ( Sequence::* )( size_t ) const )( &Sequence::Site ),
            ( boost::python::arg("index") )
        )
        .def(
            "Sites",
            ( const std::string & ( Sequence::* )(  ) const )( &Sequence::Sites )
        )
        .def(
            "RemoveGaps",
            ( void ( Sequence::* )(  ))( &Sequence::RemoveGaps ),
            "Removes all occurences of gap_char from the sequence."
        )
        .def(
            "Replace",
            ( void ( Sequence::* )( char, char ))( &Sequence::Replace ),
            ( boost::python::arg("search"), boost::python::arg("replace") ),
            "Replaces all occurences of search by replace."
        )
        .def(
            "Dump",
            ( std::string ( Sequence::* )(  ) const )( &Sequence::Dump ),
            "Prints the label and the whole sequence (possibly very long!)."
        )
    ;

    // -------------------------------------------------------------------
    //     Class SequenceSet
    // -------------------------------------------------------------------

    boost::python::class_< SequenceSet > ( "SequenceSet" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( SequenceSet::* )(  ))( &SequenceSet::clear ),
            "Deletes all sequences from the alignment."
        )
        .def(
            "FindSequence",
            ( Sequence * ( SequenceSet::* )( std::string ) const )( &SequenceSet::FindSequence ),
            ( boost::python::arg("label") ),
            "Returns a pointer to a sequence with a specific label (or nullptr, if not found)."
        )
        .def(
            "RemoveList",
            ( void ( SequenceSet::* )( std::vector< std::string >, bool ))( &SequenceSet::RemoveList ),
            ( boost::python::arg("labels"), boost::python::arg("invert")=(bool)(false) ),
            "Removes and deletes all those sequences from the Aligment whose labels are in the given list. If invert is set to true, it does the same inverted: it removes all except those in the list."
        )
        .def(
            "RemoveGaps",
            ( void ( SequenceSet::* )(  ))( &SequenceSet::RemoveGaps ),
            "Calls RemoveGaps() for every Sequence."
        )
        .def(
            "Replace",
            ( void ( SequenceSet::* )( char, char ))( &SequenceSet::Replace ),
            ( boost::python::arg("search"), boost::python::arg("replace") ),
            "Calls Replace() for every Sequence."
        )
        .def(
            "Dump",
            ( std::string ( SequenceSet::* )(  ) const )( &SequenceSet::Dump ),
            "Gives a summary of the sequences names and their lengths for this alignment."
        )
    ;

    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------

    boost::python::class_< Tree > ( "Tree", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( Tree::* )(  ))( &Tree::clear )
        )
        .def(
            "swap",
            ( void ( Tree::* )( TreeType & ))( &Tree::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "Import",
            ( void ( Tree::* )( LinkArray &, NodeArray &, EdgeArray & ))( &Tree::Import ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        .def(
            "Export",
            ( void ( Tree::* )( LinkArray &, NodeArray &, EdgeArray & ))( &Tree::Export ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        .def(
            "RootLink",
            ( LinkType * ( Tree::* )(  ) const )( &Tree::RootLink )
        )
        .def(
            "RootNode",
            ( NodeType * ( Tree::* )(  ) const )( &Tree::RootNode )
        )
        .def(
            "LinkAt",
            ( LinkType * ( Tree::* )( size_t ) const )( &Tree::LinkAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "NodeAt",
            ( NodeType * ( Tree::* )( size_t ) const )( &Tree::NodeAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "EdgeAt",
            ( EdgeType * ( Tree::* )( size_t ) const )( &Tree::EdgeAt ),
            ( boost::python::arg("index") )
        )
        .def(
            "LinkCount",
            ( size_t ( Tree::* )(  ) const )( &Tree::LinkCount ),
            "Returns the number of Links of the Tree."
        )
        .def(
            "NodeCount",
            ( size_t ( Tree::* )(  ) const )( &Tree::NodeCount ),
            "Returns the number of Nodes of the Tree."
        )
        .def(
            "EdgeCount",
            ( size_t ( Tree::* )(  ) const )( &Tree::EdgeCount ),
            "Returns the number of Edges of the Tree."
        )
        .def(
            "BeginEulertour",
            ( IteratorEulertour ( Tree::* )(  ))( &Tree::BeginEulertour )
        )
        .def(
            "BeginEulertour",
            ( IteratorEulertour ( Tree::* )( const LinkType * ))( &Tree::BeginEulertour ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginEulertour",
            ( IteratorEulertour ( Tree::* )( const NodeType * ))( &Tree::BeginEulertour ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndEulertour",
            ( IteratorEulertour ( Tree::* )(  ))( &Tree::EndEulertour )
        )
        .def(
            "BeginEulertour",
            ( ConstIteratorEulertour ( Tree::* )(  ) const )( &Tree::BeginEulertour )
        )
        .def(
            "BeginEulertour",
            ( ConstIteratorEulertour ( Tree::* )( const LinkType * ) const )( &Tree::BeginEulertour ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginEulertour",
            ( ConstIteratorEulertour ( Tree::* )( const NodeType * ) const )( &Tree::BeginEulertour ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndEulertour",
            ( ConstIteratorEulertour ( Tree::* )(  ) const )( &Tree::EndEulertour )
        )
        .def(
            "BeginPreorder",
            ( IteratorPreorder ( Tree::* )(  ))( &Tree::BeginPreorder )
        )
        .def(
            "BeginPreorder",
            ( IteratorPreorder ( Tree::* )( const LinkType * ))( &Tree::BeginPreorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginPreorder",
            ( IteratorPreorder ( Tree::* )( const NodeType * ))( &Tree::BeginPreorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndPreorder",
            ( IteratorPreorder ( Tree::* )(  ))( &Tree::EndPreorder )
        )
        .def(
            "BeginPreorder",
            ( ConstIteratorPreorder ( Tree::* )(  ) const )( &Tree::BeginPreorder )
        )
        .def(
            "BeginPreorder",
            ( ConstIteratorPreorder ( Tree::* )( const LinkType * ) const )( &Tree::BeginPreorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginPreorder",
            ( ConstIteratorPreorder ( Tree::* )( const NodeType * ) const )( &Tree::BeginPreorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndPreorder",
            ( ConstIteratorPreorder ( Tree::* )(  ) const )( &Tree::EndPreorder )
        )
        .def(
            "BeginPostorder",
            ( IteratorPostorder ( Tree::* )(  ))( &Tree::BeginPostorder )
        )
        .def(
            "BeginPostorder",
            ( IteratorPostorder ( Tree::* )( const LinkType * ))( &Tree::BeginPostorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginPostorder",
            ( IteratorPostorder ( Tree::* )( const NodeType * ))( &Tree::BeginPostorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndPostorder",
            ( IteratorPostorder ( Tree::* )(  ))( &Tree::EndPostorder )
        )
        .def(
            "BeginPostorder",
            ( ConstIteratorPostorder ( Tree::* )(  ) const )( &Tree::BeginPostorder )
        )
        .def(
            "BeginPostorder",
            ( ConstIteratorPostorder ( Tree::* )( const LinkType * ) const )( &Tree::BeginPostorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginPostorder",
            ( ConstIteratorPostorder ( Tree::* )( const NodeType * ) const )( &Tree::BeginPostorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndPostorder",
            ( ConstIteratorPostorder ( Tree::* )(  ) const )( &Tree::EndPostorder )
        )
        .def(
            "BeginLevelorder",
            ( IteratorLevelorder ( Tree::* )(  ))( &Tree::BeginLevelorder )
        )
        .def(
            "BeginLevelorder",
            ( IteratorLevelorder ( Tree::* )( const LinkType * ))( &Tree::BeginLevelorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginLevelorder",
            ( IteratorLevelorder ( Tree::* )( const NodeType * ))( &Tree::BeginLevelorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndLevelorder",
            ( IteratorLevelorder ( Tree::* )(  ))( &Tree::EndLevelorder )
        )
        .def(
            "BeginLevelorder",
            ( ConstIteratorLevelorder ( Tree::* )(  ) const )( &Tree::BeginLevelorder )
        )
        .def(
            "BeginLevelorder",
            ( ConstIteratorLevelorder ( Tree::* )( const LinkType * ) const )( &Tree::BeginLevelorder ),
            ( boost::python::arg("link") )
        )
        .def(
            "BeginLevelorder",
            ( ConstIteratorLevelorder ( Tree::* )( const NodeType * ) const )( &Tree::BeginLevelorder ),
            ( boost::python::arg("node") )
        )
        .def(
            "EndLevelorder",
            ( ConstIteratorLevelorder ( Tree::* )(  ) const )( &Tree::EndLevelorder )
        )
        .def(
            "BeginLinks",
            ( IteratorLinks ( Tree::* )(  ))( &Tree::BeginLinks )
        )
        .def(
            "EndLinks",
            ( IteratorLinks ( Tree::* )(  ))( &Tree::EndLinks )
        )
        .def(
            "BeginLinks",
            ( ConstIteratorLinks ( Tree::* )(  ) const )( &Tree::BeginLinks )
        )
        .def(
            "EndLinks",
            ( ConstIteratorLinks ( Tree::* )(  ) const )( &Tree::EndLinks )
        )
        .def(
            "BeginNodes",
            ( IteratorNodes ( Tree::* )(  ))( &Tree::BeginNodes )
        )
        .def(
            "EndNodes",
            ( IteratorNodes ( Tree::* )(  ))( &Tree::EndNodes )
        )
        .def(
            "BeginNodes",
            ( ConstIteratorNodes ( Tree::* )(  ) const )( &Tree::BeginNodes )
        )
        .def(
            "EndNodes",
            ( ConstIteratorNodes ( Tree::* )(  ) const )( &Tree::EndNodes )
        )
        .def(
            "BeginEdges",
            ( IteratorEdges ( Tree::* )(  ))( &Tree::BeginEdges )
        )
        .def(
            "EndEdges",
            ( IteratorEdges ( Tree::* )(  ))( &Tree::EndEdges )
        )
        .def(
            "BeginEdges",
            ( ConstIteratorEdges ( Tree::* )(  ) const )( &Tree::BeginEdges )
        )
        .def(
            "EndEdges",
            ( ConstIteratorEdges ( Tree::* )(  ) const )( &Tree::EndEdges )
        )
        .def(
            "FindNode",
            ( NodeType * ( Tree::* )( std::string ) const )( &Tree::FindNode ),
            ( boost::python::arg("name") )
        )
        .def(
            "MaxRank",
            ( int ( Tree::* )(  ) const )( &Tree::MaxRank )
        )
        .def(
            "IsBifurcating",
            ( bool ( Tree::* )(  ) const )( &Tree::IsBifurcating )
        )
        .def(
            "LeafCount",
            ( size_t ( Tree::* )(  ) const )( &Tree::LeafCount )
        )
        .def(
            "InnerCount",
            ( size_t ( Tree::* )(  ) const )( &Tree::InnerCount )
        )
        .def(
            "Length",
            ( double ( Tree::* )(  ) const )( &Tree::Length )
        )
        .def(
            "NodeDepthMatrix",
            ( Matrix< int > * ( Tree::* )(  ) const )( &Tree::NodeDepthMatrix )
        )
        .def(
            "NodeDepthVector",
            ( std::vector< int > ( Tree::* )( const NodeType * ) const )( &Tree::NodeDepthVector ),
            ( boost::python::arg("node")=(const NodeType *)(nullptr) )
        )
        .def(
            "NodeDistanceMatrix",
            ( Matrix< double > * ( Tree::* )(  ) const )( &Tree::NodeDistanceMatrix )
        )
        .def(
            "NodeDistanceVector",
            ( std::vector< double > ( Tree::* )( const NodeType * ) const )( &Tree::NodeDistanceVector ),
            ( boost::python::arg("node")=(const NodeType *)(nullptr) )
        )
        .def(
            "ClosestLeafDepthVector",
            ( NodeIntVectorType ( Tree::* )(  ) const )( &Tree::ClosestLeafDepthVector )
        )
        .def(
            "ClosestLeafDistanceVector",
            ( NodeDoubleVectorType ( Tree::* )(  ) const )( &Tree::ClosestLeafDistanceVector )
        )
        .def(
            "DeepestDistance",
            ( double ( Tree::* )(  ) const )( &Tree::DeepestDistance )
        )
        .def(
            "Equal",
            ( bool ( Tree::* )( const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ) const )( &Tree::Equal ),
            ( boost::python::arg("other"), boost::python::arg("comparator") )
        )
        .def(
            "HasIdenticalTopology",
            ( bool ( Tree::* )( const TreeType & ) const )( &Tree::HasIdenticalTopology ),
            ( boost::python::arg("other") )
        )
        .def(
            "Validate",
            ( bool ( Tree::* )(  ) const )( &Tree::Validate )
        )
        .def(
            "Dump",
            ( std::string ( Tree::* )(  ) const )( &Tree::Dump )
        )
        .def(
            "DumpLinks",
            ( std::string ( Tree::* )(  ) const )( &Tree::DumpLinks )
        )
        .def(
            "DumpNodes",
            ( std::string ( Tree::* )(  ) const )( &Tree::DumpNodes )
        )
        .def(
            "DumpEdges",
            ( std::string ( Tree::* )(  ) const )( &Tree::DumpEdges )
        )
        .def(
            "DumpEulertour",
            ( std::string ( Tree::* )(  ) const )( &Tree::DumpEulertour )
        )
        .def(
            "Equal",
            ( bool ( Tree::* )( const TreeType &, const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ))( &Tree::Equal ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") )
        )
        .staticmethod("Equal")

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeEdge
    // -------------------------------------------------------------------

    boost::python::class_< TreeEdge > ( "TreeEdge", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "PrimaryLink",
            ( LinkType * ( TreeEdge::* )(  ) const )( &TreeEdge::PrimaryLink ),
            "Returns the link of this edge that points towards the root."
        )
        .def(
            "SecondaryLink",
            ( LinkType * ( TreeEdge::* )(  ) const )( &TreeEdge::SecondaryLink ),
            "Returns the link of this edge that points away from the root."
        )
        .def(
            "PrimaryNode",
            ( NodeType * ( TreeEdge::* )(  ) const )( &TreeEdge::PrimaryNode )
        )
        .def(
            "SecondaryNode",
            ( NodeType * ( TreeEdge::* )(  ) const )( &TreeEdge::SecondaryNode )
        )
        .def(
            "Index",
            ( size_t ( TreeEdge::* )(  ) const )( &TreeEdge::Index ),
            "Returns the index of this Link."
        )
        .def(
            "Dump",
            ( std::string ( TreeEdge::* )(  ) const )( &TreeEdge::Dump )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorEulertour
    // -------------------------------------------------------------------

    boost::python::class_< TreeIteratorEulertour > ( "TreeIteratorEulertour", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "Link",
            ( LinkPointerType ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorLevelorder
    // -------------------------------------------------------------------

    boost::python::class_< TreeIteratorLevelorder > ( "TreeIteratorLevelorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsFirstIteration",
            ( bool ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::IsFirstIteration )
        )
        .def(
            "Depth",
            ( int ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::Depth )
        )
        .def(
            "Link",
            ( LinkPointerType ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( TreeIteratorLevelorder::* )(  ) const )( &TreeIteratorLevelorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorPostorder
    // -------------------------------------------------------------------

    boost::python::class_< TreeIteratorPostorder > ( "TreeIteratorPostorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsLastIteration",
            ( bool ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::IsLastIteration )
        )
        .def(
            "Link",
            ( LinkPointerType ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( TreeIteratorPostorder::* )(  ) const )( &TreeIteratorPostorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeIteratorPreorder
    // -------------------------------------------------------------------

    boost::python::class_< TreeIteratorPreorder > ( "TreeIteratorPreorder", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "IsFirstIteration",
            ( bool ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::IsFirstIteration )
        )
        .def(
            "Link",
            ( LinkPointerType ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::StartLink )
        )
        .def(
            "StartNode",
            ( NodePointerType ( TreeIteratorPreorder::* )(  ) const )( &TreeIteratorPreorder::StartNode )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeLink
    // -------------------------------------------------------------------

    boost::python::class_< TreeLink > ( "TreeLink", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "Next",
            ( LinkType * ( TreeLink::* )(  ) const )( &TreeLink::Next ),
            "Returns a pointer to the next link within the node."
        )
        .def(
            "Outer",
            ( LinkType * ( TreeLink::* )(  ) const )( &TreeLink::Outer ),
            "Returns a pointer to the link of the adjacent node."
        )
        .def(
            "Edge",
            ( EdgeType * ( TreeLink::* )(  ) const )( &TreeLink::Edge ),
            "Returns a pointer to the TreeEdge containing the data of this link's edge."
        )
        .def(
            "Node",
            ( NodeType * ( TreeLink::* )(  ) const )( &TreeLink::Node ),
            "Returns a pointer to the TreeNode containing the data of this link's node."
        )
        .def(
            "Index",
            ( size_t ( TreeLink::* )(  ) const )( &TreeLink::Index ),
            "Returns the index of this Link."
        )
        .def(
            "IsLeaf",
            ( bool ( TreeLink::* )(  ) const )( &TreeLink::IsLeaf ),
            "Returns true iff the node of this link is a leaf node."
        )
        .def(
            "IsInner",
            ( bool ( TreeLink::* )(  ) const )( &TreeLink::IsInner ),
            "Returns true iff the node of this link is an inner node."
        )
        .def(
            "Dump",
            ( std::string ( TreeLink::* )(  ) const )( &TreeLink::Dump ),
            "Returns a string containing dump information about this link."
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeNode
    // -------------------------------------------------------------------

    boost::python::class_< TreeNode > ( "TreeNode", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "PrimaryLink",
            ( LinkType * ( TreeNode::* )(  ) const )( &TreeNode::PrimaryLink ),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "Link",
            ( LinkType * ( TreeNode::* )(  ) const )( &TreeNode::Link ),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "BeginLinks",
            ( IteratorLinks ( TreeNode::* )(  ))( &TreeNode::BeginLinks )
        )
        .def(
            "EndLinks",
            ( IteratorLinks ( TreeNode::* )(  ))( &TreeNode::EndLinks )
        )
        .def(
            "BeginLinks",
            ( ConstIteratorLinks ( TreeNode::* )(  ) const )( &TreeNode::BeginLinks )
        )
        .def(
            "EndLinks",
            ( ConstIteratorLinks ( TreeNode::* )(  ) const )( &TreeNode::EndLinks )
        )
        .def(
            "Index",
            ( size_t ( TreeNode::* )(  ) const )( &TreeNode::Index ),
            "Returns the index of this Link."
        )
        .def(
            "Rank",
            ( int ( TreeNode::* )(  ) const )( &TreeNode::Rank )
        )
        .def(
            "IsLeaf",
            ( bool ( TreeNode::* )(  ) const )( &TreeNode::IsLeaf )
        )
        .def(
            "IsInner",
            ( bool ( TreeNode::* )(  ) const )( &TreeNode::IsInner )
        )
        .def(
            "Dump",
            ( std::string ( TreeNode::* )(  ) const )( &TreeNode::Dump )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class TreeNodeIteratorLinks
    // -------------------------------------------------------------------

    boost::python::class_< TreeNodeIteratorLinks > ( "TreeNodeIteratorLinks", boost::python::init< LinkPointerType >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "Link",
            ( LinkPointerType ( TreeNodeIteratorLinks::* )(  ))( &TreeNodeIteratorLinks::Link )
        )
        .def(
            "Node",
            ( NodePointerType ( TreeNodeIteratorLinks::* )(  ))( &TreeNodeIteratorLinks::Node )
        )
        .def(
            "Edge",
            ( EdgePointerType ( TreeNodeIteratorLinks::* )(  ))( &TreeNodeIteratorLinks::Edge )
        )
        .def(
            "StartLink",
            ( LinkPointerType ( TreeNodeIteratorLinks::* )(  ))( &TreeNodeIteratorLinks::StartLink )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;

    // -------------------------------------------------------------------
    //     Class TreeSet
    // -------------------------------------------------------------------

    boost::python::class_< TreeSet > ( "TreeSet", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlComment
    // -------------------------------------------------------------------

    boost::python::class_< XmlComment > ( "XmlComment", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlDocument
    // -------------------------------------------------------------------

    boost::python::class_< XmlDocument > ( "XmlDocument" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( XmlDocument::* )(  ))( &XmlDocument::clear )
        )
    ;

    // -------------------------------------------------------------------
    //     Class XmlElement
    // -------------------------------------------------------------------

    boost::python::class_< XmlElement > ( "XmlElement", boost::python::init<  >((  )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( XmlElement::* )(  ))( &XmlElement::clear )
        )
    ;

    // -------------------------------------------------------------------
    //     Class XmlMarkup
    // -------------------------------------------------------------------

    boost::python::class_< XmlMarkup > ( "XmlMarkup", boost::python::init<  >((  )) )
    ;

    // -------------------------------------------------------------------
    //     Class XmlProcessor
    // -------------------------------------------------------------------

    boost::python::class_< XmlProcessor > ( "XmlProcessor" )

        // Public Member Functions

        .def(
            "ToFile",
            ( bool ( XmlProcessor::* )( const std::string &, const XmlDocument & ))( &XmlProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("document") ),
            "Writes an XML file from an XmlDocument. Returns true iff successful."
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( XmlProcessor::* )( std::string &, const XmlDocument & ))( &XmlProcessor::ToString ),
            ( boost::python::arg("xml"), boost::python::arg("document") ),
            "Gives the XML string representation of a XmlDocument."
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( XmlProcessor::* )( const XmlDocument & ))( &XmlProcessor::ToString ),
            ( boost::python::arg("document") ),
            "Returns the XML representation of a XmlDocument."
        )
        .staticmethod("ToString")
    ;

    // -------------------------------------------------------------------
    //     Class XmlValue
    // -------------------------------------------------------------------

    boost::python::class_< XmlValue > ( "XmlValue" )

        // Public Member Functions

        .def(
            "TypeToString",
            ( std::string ( XmlValue::* )( const Type ))( &XmlValue::TypeToString ),
            ( boost::python::arg("t") )
        )
        .staticmethod("TypeToString")
        .def(
            "TypeToString",
            ( std::string ( XmlValue::* )(  ) const )( &XmlValue::TypeToString )
        )
        .def(
            "type",
            ( Type ( XmlValue::* )(  ) const )( &XmlValue::type )
        )
        .def(
            "IsComment",
            ( bool ( XmlValue::* )(  ))( &XmlValue::IsComment )
        )
        .def(
            "IsMarkup",
            ( bool ( XmlValue::* )(  ))( &XmlValue::IsMarkup )
        )
        .def(
            "IsElement",
            ( bool ( XmlValue::* )(  ))( &XmlValue::IsElement )
        )
    ;


