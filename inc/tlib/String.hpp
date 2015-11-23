
#ifndef TLIB_STRING_HPP
#define TLIB_STRING_HPP

#include <stdint.h>
#include <stddef.h>

#include <boost/locale.hpp>
#include <locale>
#include <string>
#include <utility>

namespace tlib {
    template <typename Allocator>
    class Utf8String {
    public:
        /**
         * \brief Create an empty Utf8String
         */
        Utf8String() :
            mData()
        { }
        
        /**
         * \brief Move a Utf8String
         * 
         * In order to move the string, the allocators have to be of the same
         * type. Further, the allocator must be copied - if you want allocators
         * with shared state they should delegate the actual operations to
         * something accessed through a shared pointer.
         * 
         * \param source The source Utf8String to move
         */
        Utf8String(Utf8String<Allocator> &&source) :
            mData(std::forward<std::basic_string<char>>(source.mData))
        { }
        
        /**
         * \brief Create a Utf8String copy of the given c-string data.
         */
        Utf8String(const char *cstr, std::locale loc = std::locale()) :
            mData(boost::locale::normalize(cstr,
                                           boost::locale::norm_default,
                                           loc))
        { }
        
        /**
         * \brief Copy the given Utf8String
         * 
         * This constructor may take Utf8Strings with possibly different
         * allocators.
         * 
         * \param source The source Utf8String to copy
         */
        template <typename Alloc>
        Utf8String(const Utf8String<Alloc> &source) :
            mData(source.mData.data())
        { }
        
        /**
         * \brief Destroy the Utf8String
         */
        ~Utf8String() { }
        
        template <typename Alloc>
        Utf8String<Allocator> & operator=(const Utf8String<Alloc> &rhs) {
            mData = rhs.mData.data();
            return *this;
        }
        Utf8String<Allocator> & operator=(const char *cstr) {
            mData = cstr;
            return *this;
        }
        
        /**
         * \brief Get the size of the string
         * 
         * The value returned by this function is the number of bytes stored
         * in this buffer.
         * 
         * To get the number of code points in the string, use the length()
         * method.
         * 
         * To get the number of characters in the string, use the characters()
         * method instead.
         * 
         * \returns The number of code points in the string.
         */
        size_t size() const {
            return mData.size();
        }
        
        /**
         * \brief Get the length of the string
         * 
         * To get the size of the buffer use the size() method instead.
         * 
         * To get the number of characters in the string, use the characters()
         * method instead.
         * 
         * \returns The number of code points in the string.
         */
        size_t length() const {
            size_t bsize = mData.size();
            size_t count = 0;
            for(size_t i = 0; i < bsize; ) {
                auto ch = mData.at(i);
                if((ch & 0x80) == 0) {
                    i += 1;
                }else if((ch & 0xE0) == 0xC0) {
                    i += 2;
                }else if((ch & 0xF0) == 0xE0) {
                    i += 3;
                }else if((ch & 0xF8) == 0xF0) { //< TODO: Test this
                    i += 4;
                }else if((ch & 0xFC) == 0xF8) { //< TODO: Test this
                    i += 5;
                }else if((ch & 0xFE) == 0xFC) { //< TODO: Test this
                    i += 6;
                }else {
                    //TODO: Error, this is not a valid utf-8 encoded string
                    return 0;
                }
                count += 1;
            }
            return count;
        }
        
        /**
         * \brief Get the capacity of the string
         * 
         * The value returned by this is the number of bytes the string buffer
         * can hold.
         * 
         * \returns The capacity of the string in bytes
         */
        size_t capacity() const {
            return mData.capacity();
        }
        
        /**
         * \brief Get the number of characters in the string
         * 
         * The value returned by this is the number of characters in the
         * string. This value is modified by combining characters in such a
         * way that
         * 
         *     characters() <= length() <= size()
         * 
         *     characters() = length() - (# of combining marks/letter pairs)
         * 
         * That is, for each regular character preceeded by a combining
         * character, the value of characters() is one less than that of
         * length().
         * 
         * \return The number of characters in the string
         */
        size_t characters(std::locale const & loc = std::locale()) const {
            // I generally don't like doing this, but not doing so causes the
            // map type to take a line, and the arguments to take 2
            namespace ba = boost::locale::boundary;
            typedef ba::segment_index<std::string::const_iterator> MapType;
            
            // This /should/ give us an object we can iterate over to
            // examine the characters in the string.
            MapType map(ba::character, mData.begin(), mData.end(), loc);
            
            size_t count = 0;
            for(auto i = map.begin(); i != map.end(); ++i) {
                count += 1;
            }
            return count;
        }
        
        /**
         * \brief Test if this string is empty
         * 
         * Functionally equivalent to
         * 
         *     size() == 0
         *     length() == 0
         *     characters() == 0
         * 
         * \return True if the string is empty.
         */
        bool empty() const {
            return mData.empty();
        }
        
        /**
         * \brief Change the size of this string to the new size
         * 
         * The characters inserted into the string are null characters '\0'.
         * 
         * \param size The new size of the string
         */
        void resize(size_t size) {
            mData.resize(size, '\0');
        }
        
        /**
         * \brief Change the size of this string to the new size and initialize
         * 
         * The characters inserted into the string are the UTF-8 character
         * given
         */
        void resize(size_t newsize, char c) {
            mData.resize(newsize, c);
        }
        /**
         * \brief Change the size of this string to the new size and initialize
         */
        template <typename Alloc>
        void resize(size_t newsize, const Utf8String<Alloc> &c) {
            //TODO: Write Me!
        }
        
        size_t reserve(size_t size) {
            mData.reserve(size);
        }
        
        /**
         * \brief 
         */
        void clear() {
            mData.clear();
        }
        
        /**
         * \brief Reduces the capacity to fit the length of the string.
         * 
         * Reduces the capacity of the string to be equal to the length of the
         * string. Unlike the standard library, the capacity of the string
         * after this operation is guaranteed to be equal to the length of the
         * string.
         */
        void shrink() {
            mData.shrink_to_fit();
        }
        
        /**
         * \brief Return a copy of this string
         * \returns A copy of this string
         */
        Utf8String copy() const {
            return Utf8String(*this);
        }
        
        /**
         * \brief Convert this string into a unicode normalized form
         * 
         * Perform a conversion of the string which normalizes the contents
         * of the string in a unicode defined way.
         * 
         * Any string which is read from the user should have this called on it
         * to prevent any problems when comparing strings.
         * 
         * \param loc The locale in which to perform this conversion
         * \returns The normalized version of this string
         */
        Utf8String normalize(std::locale loc = std::locale()) const {
            boost::locale::norm_type n = boost::locale::norm_default;
            return Utf8String(boost::locale::normalize(mData, n, loc));
        }
        
        /**
         * \brief Convert this string to lower case
         * 
         * Perform a full string lower case conversion. Every character in this
         * string is converted to its lower case equivalent if one exists.
         * 
         * Note that this is not sufficient for a semantic comparison of the
         * contents of the text.
         * 
         * If no locale is given to this function, the global locale is used.
         * 
         * \param loc The locale in which to perform this conversion
         * \returns The lower case version of this string
         */
        Utf8String lower(std::locale const & loc = std::locale()) const {
            return Utf8String(boost::locale::to_lower(mData, loc));
        }
        
        /**
         * \breif Convert this string to upper case
         * 
         * Perform a full string upper case conversion. Every character in this
         * string is converted to its upper case equivalent if one exists.
         * 
         * Note that this is not sufficent for a semantic comparison of the
         * contents of the text.
         * 
         * If no locale is given to this function, the global locale is used.
         * 
         * \param loc The locale in which to perform this conversion 
         * \return A string in which all characters are upper-case
         */
        Utf8String upper(std::locale const & loc = std::locale()) const {
            return Utf8String(boost::locale::to_upper(mData, loc));
        }
        
        /**
         * \brief Convert this string to title case
         * 
         * Perform a title-case conversion of the string, which makes the first
         * character after each word break upper case and the rest lower case.
         * 
         * If no locale is given to this string, the global locale is used.
         * 
         * \param loc The locale in which to perform this conversion
         * \returns A string which has been converted to title-case
         */
        Utf8String title(std::locale const & loc = std::locale()) const {
            return Utf8String(boost::locale::to_title(mData, loc));
        }
        
        /**
         * \brief Convert this string into a representation for comparison.
         * 
         * Perform a text fold operation, which converts the underlying unicode
         * text into a form which is intended for comparisons.
         * 
         * See http://www.w3.org/International/wiki/Case_folding for more
         * information on the subject.
         * 
         * If no locale is given to this function, the global locale is used.
         * 
         * \param loc The locale in which to perform this conversion
         * \returns A string intended for semantic comparison
         */
        Utf8String fold(std::locale const & loc = std::locale()) const {
            return Utf8String(boost::locale::fold_case(mData, loc));
        }
        
        const std::basic_string<char> & stdstring() const {
            return mData;
        }
        
        //Utf8String asUtf8() const;
        //Utf16String asUtf16() const;
        //Utf32String asUtf32() const;
    private:
        std::basic_string<char> mData;
    };
    
    template <typename ostream, typename alloc>
    ostream & operator<<(ostream &out, const Utf8String<alloc> &str) {
        return out << str.stdstring();
    }
    
    
    typedef Utf8String<std::allocator<char>> U8String;
}

#endif
