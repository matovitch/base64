#include "base64/base64.hpp"

extern "C"
{
    #include "ketopt/ketopt.h"
}

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <vector>

static const ko_longopt_t K_OPTIONS[] =
{
    { "help"    , 0, 'h' },
    { "version" , 0, 'v' },
    { "encode"  , 0, 'e' },
    { "decode"  , 0, 'd' },
    { 0         , 0,  0  } // sentinel required
};

void displayHelp(std::ostream& os)
{
    os << "base64\n\n";
    os << "Usage:\n";
    os << "    " << "-h --help       Show this screen\n";
    os << "    " << "-v --version    Display version\n";
}

void displayVersion(std::ostream& os)
{
    os << "base64, version 0.1\n";
}

static const std::size_t K_READ_BATCH_SIZE = 0x1000;

auto makeBytes(std::istream& is)
{
    std::vector<uint8_t> bytes;

    while (!is.eof())
    {
        bytes.resize(bytes.size() + K_READ_BATCH_SIZE);

        is.read(reinterpret_cast<char*>(bytes.data() + bytes.size() - K_READ_BATCH_SIZE), K_READ_BATCH_SIZE);
    }

    bytes.resize(bytes.size() - K_READ_BATCH_SIZE + is.gcount());

    return bytes;
}

void display(const std::vector<uint8_t>& bytes, std::ostream& os)
{
    for (const auto byte : bytes)
    {
        os << byte;
    }

    os << '\n';
}

void displayEncode(const std::vector<uint8_t>& srce, std::ostream& os)
{
    std::vector<uint8_t> dest;

    dest.resize(base64::encodeBufferSize(srce.size()));

    base64::encode(srce.data(),
                   srce.size(),
                   dest.data());

    display(dest, os);
}

void displayDecode(const std::vector<uint8_t>& srce, std::ostream& os)
{
    std::vector<uint8_t> dest;

    dest.resize(base64::decodeBufferSize(srce.data(),
                                         srce.size()));
    base64::decode(srce.data(),
                   srce.size(),
                   dest.data());

    display(dest, os);
}

int main(int argc, char** argv)
{
    bool hasHelp    = false;
    bool hasVersion = false;
    bool hasEncode  = false;
    bool hasDecode  = false;

    ketopt_t ketoptStatus = KETOPT_INIT;
    int ketoptOption;

    while ((ketoptOption = ketopt(&ketoptStatus, argc, argv, 0, "hved", K_OPTIONS)) != -1)
    {
        if (ketoptOption == 'h') { hasHelp    = true; }
        if (ketoptOption == 'v') { hasVersion = true; }
        if (ketoptOption == 'e') { hasEncode  = true; }
        if (ketoptOption == 'd') { hasDecode  = true; }
    }

    if (hasHelp)
    {
        displayHelp(std::cout);
        return EXIT_SUCCESS;
    }

    if (hasVersion)
    {
        displayVersion(std::cout);
        return EXIT_SUCCESS;
    }

    if (hasEncode && hasDecode)
    {
        std::cerr << "error: encode and decode options are mutually exclusives.\n";
        displayHelp(std::cerr);
        return EXIT_FAILURE;
    }

    const auto& bytes = makeBytes(std::cin);

    if (hasEncode)
    {
        displayEncode(bytes, std::cout);
    }

    if (hasDecode)
    {
        displayDecode(bytes, std::cout);
    }

    return EXIT_SUCCESS;
}
