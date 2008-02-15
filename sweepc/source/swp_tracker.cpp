#include "swp_tracker.h"
#include <stdexcept>

using namespace Sweep;
using namespace std;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor.
Tracker::Tracker()
: m_name("")
{
}

// Copy constructor.
Tracker::Tracker(const Sweep::Tracker &copy)
{
    *this = copy;
}

// Initialising constructor.
Tracker::Tracker(const std::string &name)
{
    m_name = name;
}

// Stream-reading constructor.
Tracker::Tracker(std::istream &in)
{
    Deserialize(in);
}

// Default destructor.
Tracker::~Tracker()
{
    // Nothing special to destruct.
}


// OPERATOR OVERLOADING.

// Assignment operator.
Tracker &Tracker::operator =(const Sweep::Tracker &rhs)
{
    if (this != &rhs) {
        m_name = rhs.m_name;
    }
    return *this;
}


// READ/WRITE/COPY.

// Returns a copy of the tracker variable.
Tracker *const Tracker::Clone(void) const {return new Tracker(*this);}

// Writes the object to a binary stream.
void Tracker::Serialize(std::ostream &out) const
{
    if (out.good()) {
        // Output the version ID (=0 at the moment).
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));

        // Write the length of the component name to the stream.
        unsigned int n = m_name.length();
        out.write((char*)&n, sizeof(n));

        // Write the component name to the stream.
        out.write(m_name.c_str(), n);
    } else {
        throw invalid_argument("Output stream not ready "
                               "(Sweep, Tracker::Serialize).");
    }
}

// Reads the object from a binary stream.
void Tracker::Deserialize(std::istream &in)
{
    if (in.good()) {
        // Read the output version.  Currently there is only one
        // output version, so we don't do anything with this variable.
        // Still needs to be read though.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        unsigned int n = 0;
        char *name = NULL;

        switch (version) {
            case 0:
                // Read the length of the species name.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                
                // Read the species name.
                name = new char[n];
                in.read(name, n);
                m_name.assign(name, n);
                delete [] name;

                break;
            default:
                throw runtime_error("Serialized version number is invalid "
                                    "(Sweep, Tracker::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, Tracker::Deserialize).");
    }
}
