// bslstl_hashtable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE
#define INCLUDED_BSLSTL_HASHTABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash-container with support for duplicate values.
//
//@CLASSES:
//   bslstl::HashTable : hashed-table container for user-supplied object types
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template, 'HashTable',
// implementing a value-semantic container that can be used to easily implement
// the four 'unordered' containers specified by the C++11 standard.
//
// An instantiation of 'HashTable' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of keys) and the ordered
// sequence of keys the 'HashTable' contains.  If 'HashTable' is instantiated
// with a key type that is not itself value-semantic, then it will not retain
// all of its value-semantic qualities.  In particular, if the key type cannot
// be tested for equality, then a HashTable containing that type cannot be
// tested for equality.  It is even possible to instantiate 'HashTable' with a
// key type that does not have a copy-constructor, in which case the
// 'HashTable' will not be copyable.
//
///Requirements on 'KEY_CONFIG'
///----------------------------
// The elements stored in a 'HashTable' and the key by which they are indexed
// are defined by a 'KEY_CONFIG' template type parameter.  The user-supplied
// 'KEY_CONFIG' type must provide two type aliases named 'ValueType' and
// 'KeyType' that name the type of element stored and its associated key type
// respectively.  In addition, a 'KEY_CONFIG' class shall provide a static
// member function which may be called as if it had the following signature:
//..
//  static const KeyType& extractKey(const ValueType& value);
//      // Return a non-modifiable reference to the key for the specified
//      // 'value'.
//..
// A 'HashTable' is a fully "Value-Semantic Type" (see {'bsldoc_glossary'})
// only if the configured 'ValueType' is fully value-semantic.  It is possible
// to instantiate a 'HashTable' configured with a 'ValueType' that does not
// provide a full 'HashTable' of value-semantic operations, but then some
// methods of the container may not be instantiable.  The following
// terminology, adopted from the C++11 standard, is used in the function
// documentation of 'HashTable' to describe a function's requirements for the
// 'KEY' template parameter.  These terms are also defined in
// [utility.arg.requirements] (section 17.6.3.1 of the C++11 standard).  Note
// that, in the context of a 'HashTable' instantiation, the requirements apply
// specifically to the 'HashTable's element type, 'ValueType'.
//
//: "default-constructible": The type provides a default constructor.
//:
//: "copy-constructible": The type provides a copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//
///Memory Allocation
///-----------------
// The type supplied as a HashTable's 'ALLOCATOR' template parameter determines
// how that HashTable will allocate memory.  The 'HashTable' template supports
// allocators meeting the requirements of the C++ standard allocator
// requirements ([allocator.requirements], C++11 17.6.3.5); in addition it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'HashTable' instantiation is
// 'bsl::allocator', then objects of that HashTable type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a HashTable
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it will be used to supply memory for the HashTable throughout
// its lifetime; otherwise, the HashTable will use the default allocator
// installed at the time of the HashTable's construction (see 'bslma_default').
// In addition to directly allocating memory from the indicated
// 'bslma::Allocator', a HashTable supplies that allocator's address to the
// constructors of contained objects of the configured 'ValueType' with the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Exception Safety
///----------------
// The operations of a 'HashTable' provide the strong exception guarantee
// (see {'bsldoc_glossary')) except in the presence of a hash-functor or
// equality-comparator that throws exceptions.  If either the hash-function or
// equality-comparator throws an exception from a non-const method, 'HashTable'
// provides only the basic exception guarantee, and the operation will leave
// the container in a valid but unspecified (potentially empty) state.
//
///Internal Data Structure
///-----------------------
// This implementation of a hash-table uses a single bidirectional list, to
// hold all the elements stored in the container, and the elements in this list
// are indexed by a dynamic array of buckets, each of which holds a pointer to
// the first and last element in the linked-list whose adjusted hash-values
// are equal to that bucket's index.
//
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propagate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'COMPARATOR'
// predicate.
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtable.h> header can't be included directly in \
        BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#include <bslstl_bidirectionalnodepool.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#include <bslalg_functoradapter.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#include <bslalg_hashtableimputil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>  // for swap, may need <utility> with C++11 libraries
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CMATH
#include <cmath> // for 'ceil'
#define INCLUDED_CMATH
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef> // for 'size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_LIMITS
#include <limits>  // for 'numeric_limits<size_t>'
#define INCLUDED_LIMITS
#endif

namespace BloombergLP {

namespace bslstl {

                           // ===========================
                           // class HashTable_HashWrapper
                           // ===========================

template <class FUNCTOR>
class HashTable_HashWrapper {
    // This class provides a wrapper around a functor satisfying the 'Hash'
    // requirements ({'bslstl_hash'}) such that the function call operator is
    // always declared as 'const' qualified.
    //
    // TBD Provide an optimization for the case of an empty base functor, where
    //     we can safely const_cast want calling the base class operator.
    //     
    // Note that we would only one class, not two, with C++11 variaidic
    // templates and perfect forwarding, and we could also easily detect
    // whether ot not 'FUNCTOR' provided a const-qualified 'operator()'.
  private:
    mutable FUNCTOR d_functor;

  public:
    HashTable_HashWrapper();
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that
        // has its default value.

    explicit HashTable_HashWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that is
        // a copy of the specified 'fn'.

    template <class ARG_TYPE>
    native_std::size_t operator()(const ARG_TYPE& arg) const;
        // Call the wrapped 'functor' with the specified 'arg' and return the
        // result.

    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the
        // hash functor wrapped by this object.


    void swap(HashTable_HashWrapper &other);
        // Exchange the value of this object with the specified 'other' object.
};

template <class FUNCTOR>
void swap(HashTable_HashWrapper<FUNCTOR> &a,
          HashTable_HashWrapper<FUNCTOR> &b);
    // Swap the functor wrapped by the specified 'a' object with the functor
    // wrapped by the specified 'b' object.

                           // =================================
                           // class HashTable_ComparatorWrapper
                           // =================================

template <class FUNCTOR>
class HashTable_ComparatorWrapper {
    // This class provides a wrapper around a functor that can compare two
    // values and return a 'bool', so that the function call operator is
    // always declared as 'const' qualified.
    //
    // TBD Provide an optimization for the case of an empty base functor, where
    //     we can safely const_cast want calling the base class operator.
  private:
    mutable FUNCTOR d_functor;

  public:
    HashTable_ComparatorWrapper();
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that has its default value.

    explicit HashTable_ComparatorWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that is a copy of the specified 'fn'.

    template <class ARG1_TYPE, class ARG2_TYPE>
    bool operator()(const ARG1_TYPE& arg1, const ARG2_TYPE& arg2) const;
        // Call the wrapped 'functor' with the specified 'arg1' and 'arg2' (in
        // that order) and return the result.

    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the
        // hash functor wrapped by this object.

    void swap(HashTable_ComparatorWrapper &other);
        // Exchange the value of this object with the specified 'other' object.
};

template <class FUNCTOR>
void swap(HashTable_ComparatorWrapper<FUNCTOR> &lhs,
          HashTable_ComparatorWrapper<FUNCTOR> &rhs);
    // Swap the functor wrapped by the specified 'a' object with the functor
    // wrapped by the specified 'b' object.

                           // ===============
                           // class HashTable
                           // ===============

template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR = ::bsl::allocator<typename KEY_CONFIG::ValueType> >
class HashTable {
    // This class template implements a value-semantic container type holding
    // an unordered sequence of (possibly duplicate) elements, that can be
    // rapidly accessed using their key, with the constraint on the container
    // that elements whose keys compare equal according to the specified
    // 'COMPARATOR' will be stored in a stable, contiguous sequence within
    // the container.  The value type and key type of the elements maintained
    // by a 'HashTable' are determined by aliases provided through the
    // (template parameter) type 'KEY_CONFIG'.  Elements in a 'HashTable' are
    // stored in "nodes" that are allocated using an allocator of the specified
    // 'ALLOCATOR' type (rebound to the node type), and elements are
    // constructed directly in the node using the allocator as described in
    // the C++11 standard under the allocator-aware container requirements in
    // ([container.requirements.general], C++11 23.2.1).  The (template
    // parameter) types 'HASHER' and 'COMPARATOR' shall be copy-constructible
    // function-objects.  'HASHER' shall support a function call operator
    // compatible with the following statements:
    //..
    //  HASHER              hash;
    //  KEY_CONFIG::KeyType key;
    //  std::size_t result = hash(key);
    //..
    // where the definition of the called function meets the requirements of a
    // hash function, as specified in {'bslstl_hash'}.  'COMPARATOR' shall
    // support the a function call operator compatible with the following
    // statements:
    //..
    //  COMPARATOR          compare;
    //  KEY_CONFIG::KeyType key1, key2;
    //  bool result = compare(key1, key2);
    //..
    // where the definition of the called function defines an equivalence
    // relationship on keys that is both reflexive and transitive.  The
    // 'HASHER' and 'COMPARATOR' attributes of this class are further
    // constrained, such for any two objects whose keys compare equal by
    // the comparator, shall produce the same value from the hasher.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  public:
    // TYPES
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename KEY_CONFIG::KeyType           KeyType;
    typedef typename KEY_CONFIG::ValueType         ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;

  private:
    typedef typename
                  bslalg::FunctorAdapter<HashTable_HashWrapper<HASHER> >::Type
                                                                    BaseHasher;
    typedef typename 
         bslalg::FunctorAdapter<HashTable_ComparatorWrapper<COMPARATOR> >::Type
                                                                BaseComparator;

    // PRIVATE TYPES
    struct ImplParameters : private BaseHasher, private BaseComparator
    {
        // This class holds all the parameterized parts of a 'HashTable' class,
        // efficiently exploiting the empty base optimization without adding
        // unforeseen namespace associations to the 'HashTable' class itself
        // due to the structural inheritance.
      private:
        // NOT IMPLEMENTED
        ImplParameters(const ImplParameters&); // = delete;
        ImplParameters& operator=(const ImplParameters&); // = delete;

      public:
        typedef HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>
                                                                 HashTableType;

        typedef typename HashTableType::AllocatorTraits::template
                         rebind_traits<NodeType>::allocator_type NodeAllocator;

        // These aliases simplify naming the base classes in the constructor
        typedef BaseHasher     HasherBaseType;
        typedef BaseComparator ComparatorBaseType;


        typedef BidirectionalNodePool<typename HashTableType::ValueType,
                                      NodeAllocator>               NodeFactory;

        // PUBLIC DATA
        NodeFactory  d_nodeFactory;    // nested 'struct's have public data by
                                       // convention, but should always be
                                       // accessed through the public methods.

        // CREATORS
        ImplParameters(const HASHER&     hash,
                       const COMPARATOR& compare,
                       const ALLOCATOR&  allocator);
            // Create an 'ImplParameters' object having the specified 'hash',
            // and 'compare' functors, and using the specified 'allocator' to
            // provide a 'BidirectionalNodePool'.

        ImplParameters(const ImplParameters& original,
                       const ALLOCATOR&      allocator);
            // Create an 'ImplParameters' object having the same 'hasher' and
            // 'comparator' attributes as the specified 'original', and
            // providing a 'BidirectionalNodePool' using the specified
            // 'allocator'.

        // MANIPULATORS
        NodeFactory& nodeFactory();
            // Return a modifiable reference to the 'nodeFactory' owned by
            // this object.

        void quickSwapRetainAllocators(ImplParameters& other);
            // Efficiently exchange the value and functors this object with
            // those of the specified 'other' object.  This method provides the
            // no-throw exception-safety guarantee.  The behavior is undefined
            // unless this object was created with the same allocator as
            // 'other'.

        void quickSwapExchangeAllocators(ImplParameters& other);
            // Efficiently exchange the value, functor, and allocator of this
            // object with those of the specified 'other' object.  This method
            // provides the no-throw exception-safety guarantee.

        // ACCESSORS
        const BaseComparator& comparator()  const;
            // Return a non-modifiable reference to the 'comparator' functor
            // owned by this object.

        const BaseHasher&     hasher()      const;
            // Return a non-modifiable reference to the 'hasher' functor owned
            // by this object.

        const NodeFactory&    nodeFactory() const;
            // Return a non-modifiable reference to the 'nodeFactory' owned by
            // this object.

        const COMPARATOR&     originalComparator()  const;
            // Return a non-modifiable reference to the 'comparator' functor
            // owned by this object.

        const HASHER&         originalHasher()      const;
            // Return a non-modifiable reference to the 'hasher' functor owned
            // by this object.

        native_std::size_t    hashCodeForKey(const KeyType& key) const;
            // Return the hash code for the specified 'key' using a copy of the
            // hash functor supplied at construction.  Note that this function
            // is provided as common way to resolve const_cast issues in the
            // case that the stored hash functor has a function call operator
            // that is not declared as 'const'.
    };

  private:
    // DATA
    ImplParameters      d_parameters;    // policies governing table behavior
    bslalg::HashTableAnchor
                        d_anchor;        // list root and bucket array

    SizeType            d_size;          // number of elements in this table

    SizeType            d_capacity;      // max number of elements before a
                                         // rehash is required (computed from
                                         // 'd_maxLoadFactor')

    float               d_maxLoadFactor; // maximum permitted load factor

  private:
    // PRIVATE MANIPULATORS
    void copyDataStructure(const bslalg::BidirectionalLink *cursor);
        // Copy the sequence of elements from the list starting at the
        // specified 'cursor' and having 'size' elements.  Allocate a bucket
        // array sufficiently large to store 'size' elements while respecting
        // the 'maxLoadFactor', and index the copied list into that new array
        // of hash buckets.  This hash table then takes ownership of the list
        // and bucket array.  Note that this method is intended to be called
        // from copy constructors, which will have assigned some initial
        // values for the 'size' and other attributes that may not be
        // consistent with the class invariants until after this method is
        // called.

    void quickSwapRetainAllocators(HashTable& other);
        // Efficiently exchange the value and functors this object with those
        // of the specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    void quickSwapExchangeAllocators(HashTable& other);
        // Efficiently exchange the value, functors, and allocator of this
        // object with those of the specified 'other' object.  This method
        // provides the no-throw exception-safety guarantee.

    void rehashIntoExactlyNumBuckets(SizeType newNumBuckets,
                                     SizeType capacity);
        // Re-organize this hash-table to have exactly the specified
        // 'newNumBuckets'.  This operation provides the strong exception
        // guarantee (see {'bsldoc_glossary'}) unless the 'hasher' throws, in
        // which case this operation provides the basic exception guarantee,
        // leaving the hash-table in a valid, but otherwise unspecified (and
        // potentially empty), state.  The behavior is undefined unless
        // 'size / newNumBuckets <= maxLoadFactor'.

    void removeAllImp();
        // Erase all the nodes in this table and deallocate their memory via
        // the node factory, without performing the necessary bookkeeping to
        // reflect such change.  Note that this (private) method explicitly
        // leaves the HashTable in an inconsistent state, and is expected to be
        // useful when the anchor of this hash table is about to be overwritten
        // with a new value, or when the hash table is going out of scope and
        // the extra bookkeeping is not necessary.

    void removeAllAndDeallocate();
        // Erase all the nodes in this table, and deallocate their memory via
        // the supplied node factory.  Destroy the array of buckets owned by
        // this table.  If 'd_anchor.bucketAddress()' is the default (static)
        // bucket address ('HashTable_ImpDetails::defaultBucketAddress'), as
        // it is for a default constructed hashtable, then the bucket array is
        // not destroyed.

    // PRIVATE ACCESSORS
    native_std::size_t hashCodeForNode(const bslalg::BidirectionalLink *node)
                                                                         const;
        // Return the hash code for the element stored in the specified 'node'
        // using a copy of the hash functor supplied at construction.  The
        // behavior is undefined unless 'node' points to a list node of type
        // 'bslalg::BidirectionalNode<KEY_CONFIG::ValueType>'.

    bslalg::BidirectionalLink *find(const KeyType&     key,
                                    native_std::size_t hashValue) const;
        // Return the address of the first node in this hash table having a key
        // that compares equal (according to this hash-table's 'comparator') to
        // the specified 'key'.  The behavior is undefined unless the specified
        // 'hashValue' is the hash code for the specified 'key' according to
        // the 'hasher' functor of this hash table.  Note that this function's
        // implementation relies on the supplied 'hashValue' rather than
        // recomputing it, eliminating some redundant computation for the
        // public methods.

    bslalg::HashTableBucket *getBucketAddress(SizeType bucketIndex) const;
        // Return the address of the bucket at the specified 'bucketIndex' in
        // bucket array of this hash table.  The behavior is undefined unless
        // 'bucketIndex < this->numBuckets()'.

  public:
    // CREATORS
    explicit HashTable(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty 'HashTable' object with a 'maxLoadFactor' of 1.0.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // (template parameter) type 'ALLOCATOR' is used.  Use a default
        // constructed object of the (template parameter) type 'HASHER' and a
        // default constructed object of the (template parameter) type
        // 'COMPARATOR' to organize elements in the table.  If the 'ALLOCATOR'
        // is 'bsl::allocator' (the default), then 'allocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.  No
        // memory will be allocated unless the parameterizing 'HASHER' or
        // 'COMPARATOR' allocate memory in their default constructor.

    HashTable(const HASHER&     hash,
              const COMPARATOR& compare,
              SizeType          initialNumBuckets,
              float             initialMaxLoadFactor,
              const ALLOCATOR&  allocator = ALLOCATOR());
        // Create an empty hash-table using the specified 'hash' and
        // 'compare' functors to organize elements in the table, which will
        // initially have at least the specified 'initialNumBuckets' and a
        // 'maxLoadFactor' of 'initialMaxLoadFactor'.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not supplied,
        // a default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the 'ALLOCATOR' is 'bsl::allocator' (the
        // default), then 'allocator', if supplied, shall be convertible to
        // 'bslma::Allocator *'.  If the 'ALLOCATOR' is 'bsl::allocator' and
        // 'allocator' is not supplied, the currently installed default
        // allocator will be used to supply memory.  If this constructor tries
        // to allocate a number of buckets larger than can be represented by
        // this hash table's 'SizeType', a 'std::length_error' exception will
        // be thrown.  The behavior is undefined unless
        // '0 < initialMaxLoadFactor'.  Note that more than 'initialNumBuckets'
        // buckets may be created in order to preserve the bucket allocation
        // strategy of the hash-table (but never fewer).

    HashTable(const HashTable& original);
        // Create a 'HashTable' having the same value as the specified
        // 'original', and the same 'maxLoadFactor'.  Use a copy of
        // 'original.hasher()' and a copy of 'original.comparator()' to
        // organize elements in this hash-table.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        //  select_on_container_copy_construction(original.allocator())'
        // to allocate memory.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), the currently installed default allocator will be
        // used to supply memory.  Note that this hash-table may have fewer
        // buckets than 'original', and a correspondingly higher 'loadFactor',
        // so long as 'maxLoadFactor' is not exceeded.  Note that the created
        // hash table may have a different 'numBuckets' than 'original', and a
        // correspondingly different 'loadFactor', as long as 'maxLoadFactor'
        // is not exceeded.

    HashTable(const HashTable& original, const ALLOCATOR& allocator);
        // Create a 'HashTable' having the same value and 'maxLoadFactor' as
        // the specified 'original', that will use the specified 'allocator' to
        // supply memory.  Use a copy of 'original.hasher()' and a copy of
        // 'original.comparator()' to organize elements in this hash- table.
        // Note that this hash-table may have a different 'numBuckets' than
        // 'original', and a correspondingly different 'loadFactor', as long as
        // 'maxLoadFactor' is not exceeded.

    ~HashTable();
        // Destroy this object.

    // MANIPULATORS
    HashTable& operator=(const HashTable& rhs);
        // Assign to this object the value, hasher, comparator and
        // 'maxLoadFactor' of the specified 'rhs' object, replace the allocator
        // of this object with the allocator of 'rhs' if the 'ALLOCATOR' type
        // has the trait 'propagate_on_container_copy_assignment', and return a
        // reference providing modifiable access to this object.  This method
        // requires that the parameterized 'HASHER' and 'COMPARATOR' types be
        // "copy-constructible" (see {Requirements on 'KEY_CONFIG'}).

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insert(const SOURCE_TYPE& value);
        // Insert the specified 'value' into this hash-table, and return the
        // address of the new node.  If this hash-table already contains an
        // element having the same key as 'value' (according to this hash-
        // table's 'comparator') then insert 'value' immediately before the
        // first element having the same key.  Additional buckets will be
        // allocated, as needed, to preserve the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insert(const SOURCE_TYPE&         value,
                                      bslalg::BidirectionalLink *hint);
        // Insert the specified 'value' into this hash table, and return the
        // address of the new node.  If the element stored in the node pointer
        // to by the specified 'hint' has a key that compares equal to that of
        // 'value' then 'value' will be inserted immediately preceding 'hint'
        // in the list of this hash table.  Additional buckets will be
        // allocated, as needed, to preserve the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.  The behavior is undefined unless
        // 'hint' points to a node in this hash table.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  value);
    bslalg::BidirectionalLink *insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const ValueType&    value);
        // Return the address of an element in this hash table having a key
        // that compares equal to the key of the specified 'value' using the
        // 'comparator' functor of this hash-table.  If no such element exists,
        // insert a 'value' into this hash-table and return the address of that
        // newly inserted node.  Load 'true' into the specified
        // 'isInsertedFlag' if insertion is performed, and 'false' if an
        // existing element having a matching key was found.  If this
        // hash-table contains more than one element with a matching key,
        // return the first such element (from the contiguous sequence of
        // elements having a matching key).  Additional buckets will be
        // allocated, as needed, to preserve the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.

    bslalg::BidirectionalLink *insertIfMissing(const KeyType& key);
        // Return the address of a link holding an element whose key has the
        // same value as the specified 'key' (according to this hash-table's
        // 'comparator'), and, if no such link exists, insert a new link
        // having a default value with a key that is the same as 'key'.  If
        // this hash-table contains more than one element with the supplied
        // 'key', return the first such element (from the contiguous sequence
        // of elements having a matching key).  Additional buckets will be
        // allocated, as needed, to preserve the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.

    bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);
        // Remove the specified 'node' from this hash-table, and return the
        // address of the node immediately after 'node' this hash-table (prior
        // to its removal), or a null pointer value if 'node' is the last node
        // in the table.  The behavior is undefined unless 'node' refers to a
        // node in this hash-table.

    void removeAll();
        // Remove all the elements from this hash-table.  Note that this
        // hash-table is empty after this call, but allocated memory may be
        // retained for future use.  The destructor of each (non-trivial)
        // element that is remove shall be run.

    void rehashForNumBuckets(SizeType newNumBuckets);
        // Re-organize this hash-table to have at least the specified
        // 'newNumBuckets', preserving the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.  This operation provides the strong exception guarantee (see
        // {'bsldoc_glossary'}) unless the 'hasher' throws, in which case this
        // operation provides the basic exception guarantee, leaving the
        // hash-table in a valid, but otherwise unspecified (and potentially
        // empty), state.  Note that more buckets than requested may be
        // allocated in order to preserve the bucket allocation strategy of the
        // hash table (but never fewer).

    void rehashForNumElements(SizeType numElements);
        // Re-organize this hash-table to have a sufficient number of buckets
        // to accommodate at least the specified 'numElements' without
        // exceeding the 'maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception will be
        // thrown.  This operation provides the strong exception guarantee (see
        // {'bsldoc_glossary'}) unless the 'hasher' throws, in which case this
        // operation provides the basic exception guarantee, leaving the
        // hash-table in a valid, but otherwise unspecified (and potentially
        // empty), state.

    void setMaxLoadFactor(float newMaxLoadFactor);
        // Set the maximum load factor permitted by this hash table to the
        // specified 'maxLoadFactor', where load factor is the statistical mean
        // number of elements per bucket.  If 'newMaxLoadFactor < loadFactor',
        // allocate at least enough buckets to re-establish the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to allocate a
        // number of buckets larger than can be represented by this hash
        // table's 'SizeType', a 'std::length_error' exception will be
        // thrown.  The behavior is undefined unless '0 < maxLoadFactor'.

    void swap(HashTable& other);
        // Exchange the value of this object, its 'comparator' functor, its
        // 'hasher' functor, and its 'maxLoadFactor' with those of the
        // specified 'other' object.  Additionally if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true' then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee unless any
        // of the 'comparator' or 'hasher' functors throw when swapped, leaving
        // both objects in an safely destructible, but otherwise unusable,
        // state.  The operation guarantees O[1] complexity.  The behavior is
        // undefined unless either this object was created with the same
        // allocator as 'other' or 'propagate_on_container_swap' is 'true'.

    // ACCESSORS
    ALLOCATOR allocator() const;
        // Return a copy of the allocator used to construct this hash table.
        // Note that this is not the allocator used to allocate elements for
        // this hash table, which is instead a copy of that allocator rebound
        // to allocate the nodes used by the internal data structure of this
        // hash table.

    const COMPARATOR& comparator() const;
        // Return a reference providing non-modifiable access to the
        // key-equality comparison functor used by this hash table.

    const HASHER& hasher()     const;
        // Return a reference providing non-modifiable access to the
        // hash functor used by this hash-table.

    SizeType size() const;
        // Return the number of elements in this hash table.

    SizeType maxSize() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this hash-table could possibly hold.  Note that there is no
        // guarantee that the hash-table can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    SizeType numBuckets() const;
        // Return the number of buckets contained in this hash table.

    SizeType maxNumBuckets() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this hash-table could possibly have.  Note that there is no
        // guarantee that the hash-table can successfully maintain that number
        // of buckets, or even close to that number of buckets without running
        // out of resources.

    float loadFactor() const;
        // Return the current load factor for this table.  The load factor is
        // the statical mean number of elements per bucket.

    float maxLoadFactor() const;
        // Return the maximum load factor permitted by this hash table object,
        // where the load factor is the statistical mean number of elements per
        // bucket.  Note that this hash table will enforce the maximum load
        // factor by rehashing into a larger array of buckets on any any
        // insertion operation where a successful insertion would exceed the
        // maximum load factor.  The maximum load factor may actually be less
        // than the current load factor if the maximum load factor has been
        // reset, but no insert operations have yet occurred.

    bslalg::BidirectionalLink *elementListRoot() const;
        // Return the address of the first element in this hash table, or a
        // null pointer value if this hash table is empty.

    bslalg::BidirectionalLink *find(const KeyType& key) const;
        // Return the address of a link whose key has the same value as the
        // specified 'key' (according to this hash-table's 'comparator'), and
        // a null pointer value if no such link exists.  If this hash-table
        // contains more than one element having the supplied 'key', return the
        // first such element (from the contiguous sequence of elements having
        // the same key).

    void findRange(bslalg::BidirectionalLink **first,
                   bslalg::BidirectionalLink **last,
                   const KeyType&              key) const;
        // Load into the specified 'first' and 'last' pointers the respective
        // addresses of the first and last link (in the list of elements owned
        // by this hash table) where the contained elements have a key that
        // compares equal to the specified 'key' using the 'comparator' of
        // this hash-table, and null pointers values if there are no elements
        // matching 'key'.  Note that the output values will form a closed
        // range, where both 'first' and 'last' point to links satisfying the
        // predicate (rather than a semi-open range where 'last' would point
        // to the element following the range).  Also note that this
        // hash-table ensures all elements having the same key form a
        // contiguous sequence.

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node after any nodes holding a
        // value having the same key as the specified 'first' node (according
        // to this hash-table's 'comparator'), and a null pointer value if all
        // nodes following 'first' hold values with the same key as 'first'.
        // The behavior is undefined unless 'first' is a link in this hash-
        // table.  Note that this hash-table ensures all elements having the
        // same key form a contiguous sequence.

    SizeType bucketIndexForKey(const KeyType& key) const;
        // Return the index of the bucket that would contain all the elements
        // having the specified 'key'.

    const bslalg::HashTableBucket& bucketAtIndex(SizeType index) const;
        // Return a non-modifiable reference to the 'HashTableBucket' at the
        // specified 'index' position in the array of buckets of this table.
        // The behavior is undefined unless 'index < numBuckets()'.

    SizeType countElementsInBucket(SizeType index) const;
        // Return the number elements contained in the bucket at the specified
        // 'index'.  Note that this operation has linear run-time complexity
        // with respect to the number of elements in the indexed bucket.

};

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void swap(HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& x,
          HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& y);
    // Swap both the value, the hasher, the comparator and the 'maxLoadFactor'
    // of the specified 'a' object with the value, the hasher, the comparator
    // and the 'maxLoadFactor' of the specified 'b' object.  Additionally if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true' then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees O[1] complexity.  The
    // behavior is undefined unless both objects have the same allocator or
    // 'propagate_on_container_swap' is 'true'.

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator==(
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'HashTable' objects have the same
    // value if they have the same number of elements, and for every subset of
    // elements in 'lhs' having keys that compare equal (according to that
    // hash table's 'comparator'), a corresponding subset of elements exists
    // in 'rhs', having the same number of elements, where, for some
    // permutation of the 'lhs' subset, every element in that subset compares
    // equal (using 'operator==') to the corresponding element in the 'rhs'
    // subset.  This method requires that the 'ValueType' of the parameterized
    // 'KEY_CONFIG' be "equality-comparable" (see {Requirements on
    // 'KEY_CONFIG'}).  The behavior is undefined unless both the 'hasher' and
    // 'comparator' of 'lhs' and 'rhs' return the same value for every valid
    // input.

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator!=(
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'HashTable' objects do not have
    // the same value if they do not have the same number of elements, or if,
    // for any key found in 'lhs', the subset of elements having that key
    // (according to the hash-table's 'comparator') in 'lhs' either (1) does
    // not have the same number of elements as the subset of elements having
    // that key in 'rhs', or (2) there exists no permutation of the 'lhs'
    // subset where each element compares equal (using 'operator==') to the
    // corresponding element in the 'rhs' subset.  This method requires that
    // the 'ValueType' of the parameterized 'KEY_CONFIG' be
    // "equality-comparable" (see {Requirements on 'KEY_CONFIG'}).  The
    // behavior is undefined unless both the 'hasher' and 'comparator' of 'lhs'
    // and 'rhs' return the same value for every valid input.

                    // ============================
                    // class HashTable_ArrayProctor
                    // ============================

template <class FACTORY>
class HashTable_ArrayProctor {
    // This class probably already exists in 'bslalg'
  private:
    // DATA
    FACTORY                 *d_factory;
    bslalg::HashTableAnchor *d_anchor;

  private:
    // NOT IMPLEMENTED
    HashTable_ArrayProctor(const HashTable_ArrayProctor&);
    HashTable_ArrayProctor& operator == (const HashTable_ArrayProctor&);

  public:
    HashTable_ArrayProctor(FACTORY                 *factory,
                           bslalg::HashTableAnchor *anchor);


    ~HashTable_ArrayProctor();

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.
};

                    // ===========================
                    // class HashTable_NodeProctor
                    // ===========================

template <class FACTORY>
class HashTable_NodeProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically deallocates a managed list of
    // nodes upon destruction by recursively invoking the 'deleteNode'
    // method of a supplied factory on each node.  The (template parameter)
    // type 'FACTORY' shall be provide a member function that can be called as
    // if it had the following signature:
    //..
    //  void deleteNode(bslalg::BidirectionalLink *node);
    //..

  private:
    // DATA
    FACTORY                   *d_factory;
    bslalg::BidirectionalLink *d_node;

  private:
    // NOT IMPLEMENTED
    HashTable_NodeProctor(const HashTable_NodeProctor&);
    HashTable_NodeProctor& operator=(const HashTable_NodeProctor&);

  public:
    HashTable_NodeProctor(FACTORY                   *factory,
                          bslalg::BidirectionalLink *node);
        // Create a new node-proctor that conditionally manages the specified
        // 'node' (if non-zero), and that uses the specified 'factory' to
        // destroy the node (unless released) upon its destruction.  The
        // behavior is undefined unless 'node' was created by the specified
        // 'factory'.

    ~HashTable_NodeProctor();
        // Destroy this node proctor, and delete the node that it manages (if
        // any) by invoking the 'deleteNode' method of the factory supplied at
        // construction.  If no node is currently being managed, this method
        // has no effect.

    // MANIPULATORS
    void release();
        // Release from management the node currently managed by this proctor.
        // If no object is currently being managed, this method has no effect.
};

                    // ==========================
                    // class HashTable_ImpDetails
                    // ==========================

struct HashTable_ImpDetails {
    // This utility struct provides a namespace for functions that are useful
    // when implementing a hash table.

    static size_t nextPrime(size_t n);
        // Return the next prime number greater-than or equal to the specified
        // 'n' in the increasing sequence of primes chosen to disperse hash
        // codes across buckets as uniformly as possible.  Throw a
        // 'std::length_error' exception if 'n' is greater than the last prime
        // number in the sequence.  Note that, typically, prime numbers in the
        // sequence have increasing values that reflect a growth factor (e.g.,
        // each value in the sequence may be, approximately, two times the
        // preceding value).

    static bslalg::HashTableBucket *defaultBucketAddress();
        // Return that address of a statically initialized empty bucket that
        // can be shared as the (un-owned) bucket array by all empty hash
        // tables.


    static size_t growBucketsForLoadFactor(size_t *capacity,
                                           size_t  minElements,
                                           size_t  requestedBuckets,
                                           double  maxLoadFactor);
        // Return the suggested number of buckets to index a linked list that
        // can hold as many as the specified 'minElements' without exceeding
        // the specified 'maxLoadFactor', and supporting at lead the specified
        // number of 'requestedBuckets'.  Set the specified '*capactity' to the
        // maximum length of linked list that the returned number of buckets
        // could index without exceeding the maxLoadFactor.  The behavior is
        // undefined unless '0 < maxLoadFactor', '0 < minElements' and
        // '0 < requestedBuckets'.
};

                    // ====================
                    // class HashTable_Util
                    // ====================

struct HashTable_Util {
    // This utility 'struct' provide utilities for initializing and destroying
    // bucket lists in anchors that will be managed by a 'HashTable'.  They
    // cannot migrate down to 'bslalg::HashTableImpUtil' as they rely on the
    // standard library 'bslstl_allocatortraits' for their implementation.

    // CLASS METHODS
    template<class ALLOCATOR>
    static void initAnchor(bslalg::HashTableAnchor *anchor,
                           native_std::size_t       bucketArraySize,
                           const ALLOCATOR&         allocator);
        // Load into the specified 'anchor' a (contiguous) array of buckets of
        // the specified 'bucketArraySize' using memory supplied by the
        // specified 'allocator'.  The behavior is undefined unless
        // '0 < bucketArraySize' and '0 == anchor->bucketArraySize()'.  Note
        // that this operation has no effect on 'anchor->listRootAddress()'.

    template<class ALLOCATOR>
    static void destroyBucketArray(bslalg::HashTableBucket *data,
                                   native_std::size_t       bucketArraySize,
                                   const ALLOCATOR&         allocator);
};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------
                   // class HashTable_HashWrapper
                   // ---------------------------

template <class FUNCTOR>
inline
HashTable_HashWrapper<FUNCTOR>::HashTable_HashWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_HashWrapper<FUNCTOR>::HashTable_HashWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG_TYPE>
inline
native_std::size_t
HashTable_HashWrapper<FUNCTOR>::operator()(const ARG_TYPE& arg) const
{
    return d_functor(arg);
}

template <class FUNCTOR>
inline
const FUNCTOR& HashTable_HashWrapper<FUNCTOR>::functor() const
{
    return d_functor; 
}

template <class FUNCTOR>
inline
void HashTable_HashWrapper<FUNCTOR>::swap(HashTable_HashWrapper &other)
{
    using std::swap;
    swap(d_functor, other.d_functor);
}

                   // ---------------------------------
                   // class HashTable_ComparatorWrapper
                   // ---------------------------------

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<FUNCTOR>::HashTable_ComparatorWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<FUNCTOR>::
HashTable_ComparatorWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bool
HashTable_ComparatorWrapper<FUNCTOR>::operator()(const ARG1_TYPE& arg1,
                                                 const ARG2_TYPE& arg2) const
{
    return d_functor(arg1, arg2);
}

template <class FUNCTOR>
const FUNCTOR& HashTable_ComparatorWrapper<FUNCTOR>::functor() const
{
    return d_functor;
}

template <class FUNCTOR>
inline
void
HashTable_ComparatorWrapper<FUNCTOR>::swap(HashTable_ComparatorWrapper &other)
{
    using std::swap;
    swap(d_functor, other.d_functor);
}

                    // ---------------------------
                    // class HashTable_NodeProctor
                    // ---------------------------

// CREATORS
template <class FACTORY>
inline
HashTable_NodeProctor<FACTORY>::HashTable_NodeProctor(
                                            FACTORY                   *factory,
                                            bslalg::BidirectionalLink *node)
: d_factory(factory)
, d_node(node)
{
    BSLS_ASSERT_SAFE(factory);
}

template <class FACTORY>
inline
HashTable_NodeProctor<FACTORY>::~HashTable_NodeProctor()
{
    if (d_node) {
        d_factory->deleteNode(d_node);
    }
}

// MANIPULATORS
template <class FACTORY>
inline
void HashTable_NodeProctor<FACTORY>::release()
{
    d_node = 0;
}

                    // ----------------------------
                    // class HashTable_ArrayProctor
                    // ----------------------------

// CREATORS
template <class FACTORY>
inline
HashTable_ArrayProctor<FACTORY>::HashTable_ArrayProctor(
                                           FACTORY                 *factory,
                                           bslalg::HashTableAnchor *anchor)
: d_factory(factory)
, d_anchor(anchor)
{
    BSLS_ASSERT_SAFE(factory);
    BSLS_ASSERT_SAFE(anchor);
}

template <class FACTORY>
inline
HashTable_ArrayProctor<FACTORY>::~HashTable_ArrayProctor()
{
    if (d_anchor) {
        HashTable_Util::destroyBucketArray(
                                                d_anchor->bucketArrayAddress(),
                                                d_anchor->bucketArraySize(),
                                                d_factory->allocator());

        bslalg::BidirectionalLink *root = d_anchor->listRootAddress();
        while (root) {
            bslalg::BidirectionalLink *next = root->nextLink();
            d_factory->deleteNode(root);
            root = next;
        }
    }
}

// MANIPULATORS
template <class FACTORY>
inline
void HashTable_ArrayProctor<FACTORY>::release()
{
    d_anchor = 0;
}

                    // -------------------------------
                    // class HashTable::ImplParameters
                    // -------------------------------

// CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
ImplParameters(const HASHER&        hash,
               const COMPARATOR&    compare,
               const AllocatorType& allocator)
: HasherBaseType(hash)
, ComparatorBaseType(compare)
, d_nodeFactory(allocator)
{
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
ImplParameters(const ImplParameters& original,
               const AllocatorType&  allocator)
: HasherBaseType(static_cast<const HasherBaseType&>(original))
, ComparatorBaseType(static_cast<const ComparatorBaseType&>(original))
, d_nodeFactory(allocator)
{
}

// MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
                                                                   NodeFactory&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
nodeFactory()
{
    return d_nodeFactory;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
quickSwapRetainAllocators(ImplParameters& other)
{
    using native_std::swap;  // otherwise it is hidden by this very definition!

    bslalg::SwapUtil::swap(
                   static_cast<HasherBaseType*>(this),
                   static_cast<HasherBaseType*>(bsls::Util::addressOf(other)));

    bslalg::SwapUtil::swap(
               static_cast<ComparatorBaseType*>(this),
               static_cast<ComparatorBaseType*>(bsls::Util::addressOf(other)));

    nodeFactory().swapRetainAllocators(other.nodeFactory());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
quickSwapExchangeAllocators(ImplParameters& other)
{
    using native_std::swap;  // otherwise it is hidden by this very definition!

    bslalg::SwapUtil::swap(
                   static_cast<HasherBaseType*>(this),
                   static_cast<HasherBaseType*>(bsls::Util::addressOf(other)));

    bslalg::SwapUtil::swap(
               static_cast<ComparatorBaseType*>(this),
               static_cast<ComparatorBaseType*>(bsls::Util::addressOf(other)));

    nodeFactory().swapExchangeAllocators(other.nodeFactory());
}

// OBSERVERS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
                                                                BaseComparator&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
                                                             comparator() const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
                                                                    BaseHasher&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
hasher() const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
originalComparator() const
{
    return static_cast<const BaseComparator *>(this)->functor();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
originalHasher() const
{
    return static_cast<const BaseHasher *>(this)->functor();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
native_std::size_t
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
hashCodeForKey(const KeyType& key) const
{
    return static_cast<const BaseHasher &>(*this)(key);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename
         HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
                                                                   NodeFactory&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ImplParameters::
                                                            nodeFactory() const
{
    return d_nodeFactory;
}

                    // --------------------
                    // class HashTable_Util
                    // --------------------

template <class ALLOCATOR>
inline
void HashTable_Util::initAnchor(bslalg::HashTableAnchor *anchor,
                                native_std::size_t       bucketArraySize,
                                const ALLOCATOR&         allocator)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(0 != bucketArraySize);

    typedef typename ::bsl::allocator_traits<ALLOCATOR>::template
                         rebind_traits<bslalg::HashTableBucket>::allocator_type
                                                                ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator> ArrayAllocatorTraits;
    
    ArrayAllocator reboundAllocator(allocator);

    bslalg::HashTableBucket *data =
             ArrayAllocatorTraits::allocate(reboundAllocator, bucketArraySize);

    native_std::fill_n(data, bucketArraySize, bslalg::HashTableBucket());

    anchor->setBucketArrayAddressAndSize(data, bucketArraySize);
}

template <class ALLOCATOR>
inline
void HashTable_Util::destroyBucketArray(
                                     bslalg::HashTableBucket  *data,
                                     native_std::size_t        bucketArraySize,
                                     const ALLOCATOR&          allocator)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(
                  (1  < bucketArraySize
                     && HashTable_ImpDetails::defaultBucketAddress() != data)
               || (1 == bucketArraySize
                     && HashTable_ImpDetails::defaultBucketAddress() == data));

    typedef typename ::bsl::allocator_traits<ALLOCATOR>::template
                         rebind_traits<bslalg::HashTableBucket>::allocator_type
                                                                ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator> ArrayAllocatorTraits;

    if (HashTable_ImpDetails::defaultBucketAddress() != data) {
        ArrayAllocator reboundAllocator(allocator);
        ArrayAllocatorTraits::deallocate(reboundAllocator,
                                         data,
                                         bucketArraySize);
    }
}

                        //----------------
                        // class HashTable
                        //----------------

// CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const ALLOCATOR& basicAllocator)
: d_parameters(HASHER(), COMPARATOR(), basicAllocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size()
, d_capacity()
, d_maxLoadFactor(1.0)
{
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HASHER&     hash,
          const COMPARATOR& compare,
          SizeType          initialNumBuckets,
          float             initialMaxLoadFactor,
          const ALLOCATOR&  allocator)
: d_parameters(hash, compare, allocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size()
, d_capacity(0)
, d_maxLoadFactor(initialMaxLoadFactor)
{
    BSLS_ASSERT(0.0f < initialMaxLoadFactor);

    if (0 != initialNumBuckets) {
        HashTable_Util::initAnchor(
                            &d_anchor,
                            HashTable_ImpDetails::nextPrime(initialNumBuckets),
                            allocator);
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original)
: d_parameters(
  original.d_parameters,
  AllocatorTraits::select_on_container_copy_construction(original.allocator()))
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (d_size > 0) {
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original, const ALLOCATOR& allocator)
: d_parameters(original.d_parameters, allocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (0 < d_size) {
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::~HashTable()
{
#if defined(BDE_BUILD_TARGET_SAFE_2)
    // ASSERT class invariant in SAFE_2 builds.
    BSLS_ASSERT_SAFE(bslalg::HashTableImpUtil::isWellFormed<KEY_CONFIG>(
                                                 this->d_anchor,
                                                 this->d_parameters.hasher()));

    // TBD This forces a check for corruption that should be otherwise picked
    //     up by a test driver.  It should be removed before releasing the
    //     final code.
    BSLS_ASSERT_SAFE(HashTable_ImpDetails::defaultBucketAddress());
#endif

    this->removeAllAndDeallocate();
}

// PRIVATE MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::copyDataStructure(
                                       const bslalg::BidirectionalLink *cursor)
{
    BSLS_ASSERT(0 != cursor);
    BSLS_ASSERT(d_size);

    // This function will completely replace 'this->d_anchor's state.  It is
    // the caller's responsibility to ensure this will not leak resources owned
    // only by the previous state, such as the linked list.

    // Allocate an appropriate number of buckets
    size_t capacity;
    SizeType numBuckets =
               HashTable_ImpDetails::growBucketsForLoadFactor(&capacity,
                                                              d_size,
                                                              2,
                                                              d_maxLoadFactor);

    d_anchor.setListRootAddress(0);
    HashTable_Util::initAnchor(&d_anchor, numBuckets, this->allocator());

    // create a proctor for d_anchor's allocated array, and the list to follow.
    HashTable_ArrayProctor<typename ImplParameters::NodeFactory>
                          arrayProctor(&d_parameters.nodeFactory(), &d_anchor);

    d_capacity = capacity;

    do {
        // Computing hash code depends on user-supplied code, and may throw.
        // Therefore, obtain the hash code from the node we are about to copy,
        // before any memory is allocated, so there is no risk of leaking an
        // object.  The hash code must be the same for both elements.
        native_std::size_t hashCode = this->hashCodeForNode(cursor);
        bslalg::BidirectionalLink *newNode =
                                 d_parameters.nodeFactory().cloneNode(*cursor);

        bslalg::HashTableImpUtil::insertAtBackOfBucket(&d_anchor,
                                                       newNode,
                                                       hashCode);
    }
    while ((cursor = cursor->nextLink()));

    // release the proctor
    arrayProctor.release();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapRetainAllocators(HashTable& other)
{
    BSLS_ASSERT_SAFE(this->allocator() == other.allocator());

    d_parameters.quickSwapRetainAllocators(other.d_parameters);

    using native_std::swap;  // otherwise it is hidden by this very definition!

    swap(d_anchor,        other.d_anchor);
    swap(d_size,          other.d_size);
    swap(d_capacity,      other.d_capacity);
    swap(d_maxLoadFactor, other.d_maxLoadFactor);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapExchangeAllocators(HashTable& other)
{
    d_parameters.quickSwapExchangeAllocators(other.d_parameters);

    using native_std::swap;  // otherwise it is hidden by this very definition!

    swap(d_anchor,        other.d_anchor);
    swap(d_size,          other.d_size);
    swap(d_capacity,      other.d_capacity);
    swap(d_maxLoadFactor, other.d_maxLoadFactor);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
rehashIntoExactlyNumBuckets(SizeType newNumBuckets, SizeType capacity)
{
    class Proctor {
        // An object of this proctor class guarnatees that, if an exception
        // is thrown by a user-supplied hash functor, the container remains in
        // a valid, useable (but unspecified) state.  In fact, that state will
        // be an empty, as there is no reliable way to re-index a bucket array
        // if the hash functor is throwing, and the array is potentially
        // corrupted following a failed ImpUtil::rehash call.

      private:
        HashTable               *d_this;
        bslalg::HashTableAnchor *d_originalAnchor;
        bslalg::HashTableAnchor *d_newAnchor;

#if !defined(BSLS_PLATFORM_CMP_MSVC)           // Microsoft warns if these
        Proctor(const Proctor&); // = delete;  // methods are declared private.
        Proctor& operator=(const Proctor&); // = delete;
#endif

      public:
        Proctor(HashTable               *table,
                bslalg::HashTableAnchor *originalAnchor,
                bslalg::HashTableAnchor *newAnchor)
        : d_this(table)
        , d_originalAnchor(originalAnchor)
        , d_newAnchor(newAnchor)
        {
            BSLS_ASSERT(table);
            BSLS_ASSERT(originalAnchor);
            BSLS_ASSERT(newAnchor);
        }

        ~Proctor()
        {
            if (d_originalAnchor) {
                // Not dismissed, and the newAnchor now holds the correct
                // list-root.
                d_originalAnchor->setListRootAddress(
                                               d_newAnchor->listRootAddress());
                d_this->removeAll();
            }
            // Always destroy the spare anchor's bucket array at the end of
            // scope.  On a non-exceptional run, this will effectively be the
            // original bucket-array, as the anchors are swapped.
            HashTable_Util::destroyBucketArray(
                                             d_newAnchor->bucketArrayAddress(),
                                             d_newAnchor->bucketArraySize(),
                                             d_this->allocator());
        }

        void dismiss() { d_originalAnchor = 0; }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Now that 'anchor' is not default constructible, we take a copy of
    // the anchor in the table.  Would it be better for 'initAnchor' to
    // be replaced with a 'createArrayOfEmptyBuckets' function, and we use
    // the result to construct the 'newAnchor'?
    bslalg::HashTableAnchor newAnchor(0, 0, 0);
    HashTable_Util::initAnchor(&newAnchor,
                               newNumBuckets,
                               this->allocator());

    Proctor cleanUpIfUserHashThrows(this, &d_anchor, &newAnchor);

    if (d_anchor.listRootAddress()) {
        bslalg::HashTableImpUtil::rehash<KEY_CONFIG>(
                                          &newAnchor,
                                          this->d_anchor.listRootAddress(),
                                          this->d_parameters.hasher());
    }

    cleanUpIfUserHashThrows.dismiss();

    d_anchor.swap(newAnchor);
    d_capacity = capacity;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllAndDeallocate()
{
    this->removeAllImp();
    HashTable_Util::destroyBucketArray(d_anchor.bucketArrayAddress(),
                                       d_anchor.bucketArraySize(),
                                       this->allocator());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllImp()
{
    typedef bslalg::BidirectionalLink BidirectionalLink;

    // Doing too much book-keeping of hash table - look for a more efficient
    // dispose-as-we-walk, that simply resets table.Anchor.next = 0, and
    // assigns the buckets index all null pointers
    if (BidirectionalLink *root = d_anchor.listRootAddress()) {
        BidirectionalLink *next;
        do {
            next = root->nextLink();
            d_parameters.nodeFactory().deleteNode((NodeType *)root);
        }
        while((root = next));
    }
}

// PRIVATE ACCESSORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
native_std::size_t
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hashCodeForNode(
                                   const bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);

    const KeyType& k = bslalg::HashTableImpUtil::extractKey<KEY_CONFIG>(node);
    return d_parameters.hashCodeForKey(k);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::HashTableBucket *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::getBucketAddress(
                                                    SizeType bucketIndex) const
{
    BSLS_ASSERT_SAFE(bucketIndex < this->numBuckets());

    return d_anchor.bucketArrayAddress() + bucketIndex;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                                const KeyType& key,
                                                size_t         hashValue) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(
                                                     d_anchor,
                                                     key,
                                                     d_parameters.comparator(),
                                                     hashValue);
}

// MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::operator=(
                                                          const HashTable& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {

        if (AllocatorTraits::propagate_on_container_copy_assignment::VALUE) {
            HashTable other(rhs, rhs.allocator());
            quickSwapExchangeAllocators(other);
        }
        else {
            HashTable other(rhs, this->allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
{
    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the 'find' time.
    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Create a node having the new 'value' we want to insert into the table.
    // We can extract the 'key' from this value without accidentally creating
    // a temporary (using the default allocator for any dynanmic memory).
    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // This node needs wrapping in a proctor, in case either of the user-
    // supplied functors throws an exception.
    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Now we can search for the node in the table, being careful to compute
    // the hash value only once.
    const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(newNode);
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);

    if (!position) {
        ImpUtil::insertAtFrontOfBucket(&d_anchor, newNode, hashCode);
    }
    else {
        ImpUtil::insertAtPosition(&d_anchor, newNode, hashCode, position);
    }
    nodeProctor.release();

    ++d_size;

    return newNode;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insert(
                                              const SOURCE_TYPE&         value,
                                              bslalg::BidirectionalLink *hint)
{
    BSLS_ASSERT(hint);

    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the potential 'find' time later.
    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Next we must create the node, to avoid making a temporary of 'ValueType'
    // from the object of template parameter 'SOURCE_TYPE'.
    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // There is potential for the user-supplied hasher and comparator to throw,
    // so now we need to manage our 'newNode' with a proctor.
    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Insert logic, first test the hint
    const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(newNode);
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    if (!d_parameters.comparator()(key,
                                   ImpUtil::extractKey<KEY_CONFIG>(hint))) {
        hint = this->find(key, hashCode);
    }

    if (!hint) {
        ImpUtil::insertAtFrontOfBucket(&d_anchor, newNode, hashCode);
    }
    else {
        ImpUtil::insertAtPosition(&d_anchor, newNode, hashCode, hint);
    }
    nodeProctor.release();

    ++d_size;

    return newNode;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                              bool             *isInsertedFlag,
                                              const ValueType&  value)
{
    BSLS_ASSERT(isInsertedFlag);

    const KeyType& key = KEY_CONFIG::extractKey(value);
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);

    *isInsertedFlag = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        position = d_parameters.nodeFactory().createNode(value);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }

    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  value)
{
    BSLS_ASSERT(isInsertedFlag);

    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the potential 'find' time later.
    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Next we must create the node, to avoid making a temporary of 'ValueType'
    // from the object of template parameter 'SOURCE_TYPE'.
    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // There is potential for the user-supplied hasher and comparator to throw,
    // so now we need to manage our 'newNode' with a proctor.
    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Insert logic, first test the hint
    const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(newNode);
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);

    *isInsertedFlag = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        ImpUtil::insertAtFrontOfBucket(&d_anchor, position, hashCode);
        nodeProctor.release();

        ++d_size;
        position = newNode;
    }

    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                                            const KeyType& key)
{
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);
    if (!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        position = d_parameters.nodeFactory().createNode(
                                       key, typename ValueType::second_type());

        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }
    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashForNumBuckets(
                                                        SizeType newNumBuckets)
{
    if (newNumBuckets > this->numBuckets()) {
        // Compute a "good" number of buckets, e.g., pick a prime number
        // from a sorted array of exponentially increasing primes.

        size_t capacity;
        SizeType numBuckets =
               HashTable_ImpDetails::growBucketsForLoadFactor(
                                         &capacity,
                                         native_std::max<SizeType>(d_size, 1u),
                                         newNumBuckets,
                                         d_maxLoadFactor);

        this->rehashIntoExactlyNumBuckets(numBuckets, capacity);
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashForNumElements(
                                                          SizeType numElements)
{
    if (numElements > d_capacity) {
        // Compute a "good" number of buckets, e.g., pick a prime number
        // from a sorted array of exponentially increasing primes.

        size_t capacity;
        SizeType numBuckets =
             HashTable_ImpDetails::growBucketsForLoadFactor(&capacity,
                                                            numElements,
                                                            this->numBuckets(),
                                                            d_maxLoadFactor);

        this->rehashIntoExactlyNumBuckets(numBuckets, capacity);
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::remove(
                                               bslalg::BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->previousLink()
                  || d_anchor.listRootAddress() == node);

    bslalg::BidirectionalLink *result = node->nextLink();

    bslalg::HashTableImpUtil::remove(&d_anchor,
                                     node,
                                     hashCodeForNode(node));
    --d_size;

    d_parameters.nodeFactory().deleteNode((NodeType *)node);

    return result;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAll()
{
    this->removeAllImp();
    native_std::memset(
                 d_anchor.bucketArrayAddress(),
                 0,
                 sizeof(bslalg::HashTableBucket) * d_anchor.bucketArraySize());

    d_anchor.setListRootAddress(0);
    d_size = 0;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::setMaxLoadFactor(
                                                           float maxLoadFactor)
{
    BSLS_ASSERT_SAFE(0.0f < maxLoadFactor);

    if (d_capacity > 0) {
        size_t capacity;
        SizeType numBuckets =
               HashTable_ImpDetails::growBucketsForLoadFactor(&capacity,
                                                              d_size,
                                                              this->numBuckets(),
                                                              maxLoadFactor);

        this->rehashIntoExactlyNumBuckets(numBuckets, capacity);
    }

    d_maxLoadFactor = maxLoadFactor;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::swap(HashTable& other)
{
    if (AllocatorTraits::propagate_on_container_swap::VALUE) {
        quickSwapExchangeAllocators(other);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        // backward compatible behavior: swap with copies
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               d_parameters.nodeFactory().allocator() ==
               other.d_parameters.nodeFactory().allocator())) {
            quickSwapRetainAllocators(other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            HashTable thisCopy(*this, other.allocator());
            HashTable otherCopy(other, this->allocator());

            quickSwapRetainAllocators(otherCopy);
            other.quickSwapRetainAllocators(thisCopy);
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_parameters.originalComparator();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasher() const
{
    return d_parameters.originalHasher();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
ALLOCATOR HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
                                                              allocator() const
{
    return d_parameters.nodeFactory().allocator();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::size() const
{
    return d_size;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxSize() const
{
    return native_std::numeric_limits<SizeType>::max();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::elementListRoot() const
{
    return d_anchor.listRootAddress();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                                      const KeyType& key) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(
                                             d_anchor,
                                             key,
                                             d_parameters.comparator(),
                                             d_parameters.hashCodeForKey(key));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable< KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findRange(
                                         bslalg::BidirectionalLink **first,
                                         bslalg::BidirectionalLink **last,
                                         const KeyType&              key) const
{
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);

    *first = this->find(key);
    *last  = *first
           ? this->findEndOfRange(*first)
           : 0;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findEndOfRange(
                                        bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    typedef bslalg::HashTableImpUtil ImpUtil;
    const KeyType& k = ImpUtil::extractKey<KEY_CONFIG>(first);
    while ((first = first->nextLink()) &&
           d_parameters.comparator()(k,ImpUtil::extractKey<KEY_CONFIG>(first)))
    {
        // This loop body is intentionally left blank.
    }
    return first;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::numBuckets() const
{
    return d_anchor.bucketArraySize();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxNumBuckets() const
{
    return this->max_size();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::countElementsInBucket(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return bucketAtIndex(index).countElements();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketIndexForKey(
                                                      const KeyType& key) const
{
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    return bslalg::HashTableImpUtil::computeBucketIndex(
                                                   hashCode,
                                                   d_anchor.bucketArraySize());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const bslalg::HashTableBucket&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketAtIndex(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return d_anchor.bucketArrayAddress()[index];
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::loadFactor() const
{
    return static_cast<float>((double)size() / this->numBuckets());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

}  // close package namespace

//----------------------------------------------------------------------------
//                  free functions and operators
//----------------------------------------------------------------------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
bslstl::swap(bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& x,
             bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& y)
{
    x.swap(y);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool bslstl::operator==(
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs)
{
    // The template bloat of this function can be significantly reduced
    // What matters is that the two hash tables:
    // i/   are the same size
    // ii/  have lists that are permutations of each other according to the
    //      element's 'operator=='
    // This means that the implementation should be independent of all four
    // template parameters, but will depend on VALUE_TYPE deduced from the
    // KEY_CONFIG.  Otherwise, after the initial size comparison, the rest
    // depends only on the anchors.
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::size_type SizeType;
    typedef bslalg::HashTableImpUtil ImpUtil;


    // First test - are the containers the same size?
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }
    bslalg::BidirectionalLink *cursor = lhs.elementListRoot();
    if (!cursor) {
        // containers are the same size, and empty.
        return true;                                                  // RETURN
    }

    while (cursor) {
        bslalg::BidirectionalLink *rhsFirst =
                             rhs.find(ImpUtil::extractKey<KEY_CONFIG>(cursor));
        if (!rhsFirst) {
            return false;  // no matching key                         // RETURN
        }

        bslalg::BidirectionalLink *endRange = lhs.findEndOfRange(cursor);
        bslalg::BidirectionalLink *rhsLast  = rhs.findEndOfRange(rhsFirst);

        // Check the key-groups have the same length - a quick-fail test.
        bslalg::BidirectionalLink *endWalker = cursor->nextLink();
        bslalg::BidirectionalLink *rhsWalker = rhsFirst->nextLink();

        while (endWalker != endRange) {


            if (rhsWalker == rhsLast) {
                return false;   // different length subsequences      // RETURN
            }
            endWalker = endWalker->nextLink();
            rhsWalker = rhsWalker->nextLink();
        }

        if (rhsWalker != rhsLast) {
            return false;   // different length subsequences          // RETURN
        }

        // Efficiently compare identical prefixes: O(N) if sequences
        // have the same elements in the same order.  Note that comparison of
        // values in nodes is tested using 'operator==' and not the
        // key-equality comparator stored in the hash table.
        while (cursor != endRange &&
                 (ImpUtil::extractValue<KEY_CONFIG>(cursor) ==
                  ImpUtil::extractValue<KEY_CONFIG>(rhsFirst)))
        {
            cursor   = cursor->nextLink();
            rhsFirst = rhsFirst->nextLink();
        }

        if (cursor == endRange) {
            continue;
        }


        // Now comes the harder part of validating one subsequence is a
        // permutation of another, by counting elements that compare equal
        // using the equality operator, 'operator=='.
        // Note that lhs code could be simplified for hash-tables with unique
        // keys, as we can omit the counting-scan, and merely test for any
        // match within the rhs range.  Trade off the ease of a single
        // well-tested code path, vs. the importance of an efficient operator==
        // for hash containers.  lhs is currently the only place the table
        // would care about uniqueness, and risk different table types for
        // unique vs. multi-containers.  Note again that comparison of values
        // in nodes is tested using 'operator==' and not the key-equality
        // comparator stored in the hash tables.
        for (bslalg::BidirectionalLink *marker = cursor;
             marker != endRange;
             marker = marker->nextLink())
        {
            const ValueType& valueAtMarker =
                                    ImpUtil::extractValue<KEY_CONFIG>(marker);

            if (cursor != marker) {  // skip on first pass only
                // Check if the value at 'marker' has already be seen.

                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                 ImpUtil::extractValue<KEY_CONFIG>(scanner) != valueAtMarker) {
                    scanner = scanner->nextLink();
                }
                if (scanner != marker) {
                    // We have seen 'lhs' one before.
                    continue;
                }
            }

            SizeType matches = 0;
            for (bslalg::BidirectionalLink *scanner = rhsFirst;
                 scanner != rhsLast;
                 scanner = scanner->nextLink()) {
                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
                                                               valueAtMarker) {
                    ++matches;
                }
            }
            if (!matches) {
                return false;                                         // RETURN
            }

            // Remember, *scanner is by definition a good match
            for (bslalg::BidirectionalLink *scanner = marker->nextLink();
                 scanner != endRange;
                 scanner = scanner->nextLink()) {

                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
                                                               valueAtMarker) {
                    if (!--matches) {  // equal matches, but excluding initial
                        return false;                                 // RETURN
                    }
                }
            }
            if (1 != matches) {
                return false;                                         // RETURN
            }
        }
        cursor = endRange;
    }
    return true;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bool bslstl::operator!=(
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& a,
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& b)
{
    return !(a == b);
}

template <class FUNCTOR>
inline
void bslstl::swap(bslstl::HashTable_HashWrapper<FUNCTOR> &a,
                  bslstl::HashTable_HashWrapper<FUNCTOR> &b)
{
    a.swap(b);
}

template <class FUNCTOR>
inline
void bslstl::swap(bslstl::HashTable_ComparatorWrapper<FUNCTOR> &a,
                  bslstl::HashTable_ComparatorWrapper<FUNCTOR> &b)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for HashTable:
//: o A HashTable is bitwise moveable if the both functors and the allocator
//:     are bitwise moveable.
//: o A HashTable uses 'bslma' allocators if the parameterized 'ALLOCATOR' is
//:     convertible from 'bslma::Allocator*'.

namespace bslma
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bslstl::HashTable<KEY_CONFIG,
                                            HASHER,
                                            COMPARATOR,
                                            ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type
{};

}  // close namespace bslma

namespace bslmf
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct IsBitwiseMoveable<bslstl::HashTable<KEY_CONFIG,
                                           HASHER,
                                           COMPARATOR,
                                           ALLOCATOR> >
: bsl::integral_constant< bool, bslmf::IsBitwiseMoveable<HASHER>::value
                             && bslmf::IsBitwiseMoveable<COMPARATOR>::value
                             && bslmf::IsBitwiseMoveable<ALLOCATOR>::value>
{};

}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
