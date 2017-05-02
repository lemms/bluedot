// bluedot.cpp
// Copyright Laurence Emms 2017

#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "../generator/alphablendop.h"
#include "../generator/alphatocolorop.h"
#include "../generator/colortoalphaop.h"
#include "../generator/fbmop.h"
#include "../generator/fillop.h"
#include "../generator/gradientop.h"
#include "../generator/greaterthanop.h"
#include "../generator/lessthanop.h"
#include "../generator/multiplyop.h"
#include "../generator/noiseop.h"
#include "../generator/swapop.h"
#include "../generator/generator.h"

namespace po = boost::program_options;
namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

auto parse_properties(pt::ptree& property_tree, size_t& width, size_t& height, size_t& seed) -> bool
{
    try
    {
        size_t pt_width = property_tree.get<size_t>("map.width");
        width = pt_width;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find width in configuration file, defaulting to " << width << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read width in configuration file, defaulting to " << width << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unhandled exception when reading width in configuration file.\n";
        return false;
    }

    try
    {
        size_t pt_height = property_tree.get<size_t>("map.height");
        height = pt_height;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find height in configuration file, defaulting to " << height << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read height in configuration file, defaulting to " << height << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unhandled exception when reading height in configuration file.\n";
        return false;
    }

    try
    {
        size_t pt_seed = property_tree.get<size_t>("map.seed");
        seed = pt_seed;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find seed in configuration file, defaulting to " << seed << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read seed in configuration file, defaulting to " << seed << "\n";
        std::cerr << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unhandled exception when reading seed in configuration file.\n";
        return false;
    }
    return true;
}

template <typename Real>
auto create_layers(pt::ptree& property_tree, bluedot::Generator<Real>& generator, size_t width, size_t height) -> bool
{
    bool base_layer_found = false;
    try
    {
        BOOST_FOREACH(pt::ptree::value_type &v, property_tree.get_child("map.layers"))
        {
            std::string name;
            size_t channels{ 4 };
            try
            {
                std::string pt_name = v.second.get<std::string>("name");
                if (pt_name == "base")
                {
                    base_layer_found = true;
                }
                name = pt_name;
            }
            catch (pt::ptree_bad_path& e)
            {
                std::cerr << "Unable to find layer name in configuration file.\n";
                std::cerr << e.what() << "\n";
                continue;
            }
            catch (pt::ptree_bad_data& e)
            {
                std::cerr << "Unable to read layer in configuration file.\n";
                std::cerr << e.what() << "\n";
                continue;
            }

            try
            {
                size_t pt_channels = v.second.get<size_t>("channels");
                channels = pt_channels;
            }
            catch (pt::ptree_bad_path& e)
            {
                std::cerr << "Unable to find channels in configuration file, defaulting to " << channels << ".\n";
                std::cerr << e.what() << "\n";
            }
            catch (pt::ptree_bad_data& e)
            {
                std::cerr << "Unable to read channels in configuration file, defaulting to " << channels << ".\n";
                std::cerr << e.what() << "\n";
            }
            generator.create_layer(name, width, height, channels);
            std::cout << "Created layer " << name << " with " << channels << " channels.\n";
        }
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find layers in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read layers in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "Unhandled exception when reading layers in configuration file.\n";
        return false;
    }

    if (!base_layer_found)
    {
        std::cerr << "Error: No base layer found.\n";
        return false;
    }

    return true;
}

template <typename Real>
auto apply_unary_operator(const std::string& type, pt::ptree::value_type &v, bluedot::Generator<Real>& generator, bluedot::UnaryOperator<Real>& unary_operator) -> bool
{
    // read layer
    std::string layer;
    try
    {
        std::string pt_layer = v.second.get<std::string>("layer");
        layer = pt_layer;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find operator layer in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read operator layer in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    // search for mask
    boost::optional<std::string> pt_mask{v.second.get_optional<std::string>("mask")};
    bool result{true};
    if (pt_mask)
    {
        result = generator.apply_unary_operator(layer, unary_operator, *pt_mask);
    }
    else
    {
        result = generator.apply_unary_operator(layer, unary_operator);
    }

    std::cout << "Applied " << type << " operator to layer " << layer << "\n";
    return result;
}

template <typename Real>
auto apply_binary_operator(const std::string& type, pt::ptree::value_type &v, bluedot::Generator<Real>& generator, bluedot::BinaryOperator<Real>& binary_operator) -> bool
{
    // read layers
    std::string layer0;
    try
    {
        std::string pt_layer0 = v.second.get<std::string>("layer0");
        layer0 = pt_layer0;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find operator layer0 in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read operator layer0 in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    std::string layer1;
    try
    {
        std::string pt_layer1 = v.second.get<std::string>("layer1");
        layer1 = pt_layer1;
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find operator layer1 in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read operator layer1 in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    // search for mask
    boost::optional<std::string> pt_mask{v.second.get_optional<std::string>("mask")};
    bool result{true};
    if (pt_mask)
    {
        result = generator.apply_binary_operator(layer0, layer1, binary_operator, *pt_mask);
    }
    else
    {
        result = generator.apply_binary_operator(layer0, layer1, binary_operator);
    }

    std::cout << "Applied " << type << " operator to layers " << layer0 << " and " << layer1 << "\n";
    return result;
}

enum Format
{
    FormatReal, FormatChar, FormatPercent
};

template <typename Real>
auto parse_multiplier_scale_and_offset(pt::ptree::value_type &v, std::vector<Real>& multiplier, Real& scale, Real& offset) -> void
{
    multiplier.clear();
    Format format{FormatReal};
    boost::optional<std::string> pt_format = v.second.get_optional<std::string>("multiplier.format");
    if (pt_format)
    {
        if (*pt_format == "Real")
        {
            format = FormatReal;
        }
        else if (*pt_format == "Char")
        {
            format = FormatChar;
        }
        else if (*pt_format == "Percent")
        {
            format = FormatPercent;
        }
        else
        {
            std::cerr << "Unknown multiplier format.\n";
        }
    }

    boost::optional<Real> pt_a = v.second.get_optional<Real>("multiplier.a");
    if (pt_a)
    {
        if (format == FormatReal)
        {
            multiplier.push_back(*pt_a);
        }
        else if (format == FormatChar)
        {
            multiplier.push_back(*pt_a / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            multiplier.push_back(*pt_a / static_cast<Real>(100));
        }
    }
    else
        multiplier.push_back(static_cast<Real>(1.0));

    boost::optional<Real> pt_r = v.second.get_optional<Real>("multiplier.r");
    if (pt_r)
    {
        if (format == FormatReal)
        {
            multiplier.push_back(*pt_r);
        }
        else if (format == FormatChar)
        {
            multiplier.push_back(*pt_r / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            multiplier.push_back(*pt_r / static_cast<Real>(100));
        }
    }
    else
        multiplier.push_back(static_cast<Real>(1.0));

    boost::optional<Real> pt_g = v.second.get_optional<Real>("multiplier.g");
    if (pt_g)
    {
        if (format == FormatReal)
        {
            multiplier.push_back(*pt_g);
        }
        else if (format == FormatChar)
        {
            multiplier.push_back(*pt_g / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            multiplier.push_back(*pt_g / static_cast<Real>(100));
        }
    }
    else
        multiplier.push_back(static_cast<Real>(1.0));

    boost::optional<Real> pt_b = v.second.get_optional<Real>("multiplier.b");
    if (pt_b)
    {
        if (format == FormatReal)
        {
            multiplier.push_back(*pt_b);
        }
        else if (format == FormatChar)
        {
            multiplier.push_back(*pt_b / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            multiplier.push_back(*pt_b / static_cast<Real>(100));
        }
    }
    else
        multiplier.push_back(static_cast<Real>(1.0));

    boost::optional<Real> pt_scale = v.second.get_optional<Real>("scale");
    if (pt_scale)
        scale = *pt_scale;

    boost::optional<Real> pt_offset = v.second.get_optional<Real>("offset");
    if (pt_offset)
        offset = *pt_offset;
}

template <typename Real>
auto parse_level(pt::ptree::value_type &v, std::vector<Real>& level) -> void
{
    level.clear();

    Format format{FormatReal};
    boost::optional<std::string> pt_format = v.second.get_optional<std::string>("level.format");
    if (pt_format)
    {
        if (*pt_format == "Real")
        {
            format = FormatReal;
        }
        else if (*pt_format == "Char")
        {
            format = FormatChar;
        }
        else if (*pt_format == "Percent")
        {
            format = FormatPercent;
        }
        else
        {
            std::cerr << "Unknown level format.\n";
        }
    }

    boost::optional<Real> pt_a = v.second.get_optional<Real>("level.a");
    if (pt_a)
    {
        if (format == FormatReal)
        {
            level.push_back(*pt_a);
        }
        else if (format == FormatChar)
        {
            level.push_back(*pt_a / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            level.push_back(*pt_a / static_cast<Real>(100));
        }
    }
    else
        level.push_back(static_cast<Real>(0.0));

    boost::optional<Real> pt_r = v.second.get_optional<Real>("level.r");
    if (pt_r)
    {
        if (format == FormatReal)
        {
            level.push_back(*pt_r);
        }
        else if (format == FormatChar)
        {
            level.push_back(*pt_r / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            level.push_back(*pt_r / static_cast<Real>(100));
        }
    }
    else
        level.push_back(static_cast<Real>(0.0));

    boost::optional<Real> pt_g = v.second.get_optional<Real>("level.g");
    if (pt_g)
    {
        if (format == FormatReal)
        {
            level.push_back(*pt_g);
        }
        else if (format == FormatChar)
        {
            level.push_back(*pt_g / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            level.push_back(*pt_g / static_cast<Real>(100));
        }
    }
    else
        level.push_back(static_cast<Real>(0.0));

    boost::optional<Real> pt_b = v.second.get_optional<Real>("level.b");
    if (pt_b)
    {
        if (format == FormatReal)
        {
            level.push_back(*pt_b);
        }
        else if (format == FormatChar)
        {
            level.push_back(*pt_b / static_cast<Real>(256));
        }
        else if (format == FormatPercent)
        {
            level.push_back(*pt_b / static_cast<Real>(100));
        }
    }
    else
        level.push_back(static_cast<Real>(0.0));
}

template <typename Real>
auto apply_operators(pt::ptree& property_tree, bluedot::Generator<Real>& generator, size_t seed) -> bool
{
    using generator_type = boost::variate_generator<boost::mt19937, boost::uniform_real<>>;
    boost::mt19937 rng{seed};
    boost::uniform_real<> range{-1.0, 1.0};
    generator_type random_number_generator{rng, range};

    try
    {
        BOOST_FOREACH(pt::ptree::value_type &v, property_tree.get_child("map.operators"))
        {
            std::string type;
            try
            {
                std::string pt_type = v.second.get<std::string>("type");
                type = pt_type;
            }
            catch (pt::ptree_bad_path& e)
            {
                std::cerr << "Unable to find operator type in configuration file.\n";
                std::cerr << e.what() << "\n";
                continue;
            }
            catch (pt::ptree_bad_data& e)
            {
                std::cerr << "Unable to read operator type in configuration file.\n";
                std::cerr << e.what() << "\n";
                continue;
            }

            bool result{true};
            if (type == "AlphaBlendOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::AlphaBlendOperator<Real> alpha_blend_operator{multiplier, scale, offset};
                result = apply_binary_operator(type, v, generator, alpha_blend_operator);
            }
            else if (type == "AlphaToColorOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::AlphaToColorOperator<Real> alpha_to_color_operator{multiplier, scale, offset};
                result = apply_unary_operator(type, v, generator, alpha_to_color_operator);
            }
            else if (type == "ColorToAlphaOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::ColorToAlphaOperator<Real> color_to_alpha_operator{multiplier, scale, offset};
                result = apply_unary_operator(type, v, generator, color_to_alpha_operator);
            }
            else if (type == "FBMOperator")
            {
                size_t octaves{4};
                Real exponent{static_cast<Real>(2.0)};
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                boost::optional<size_t> pt_octaves = v.second.get_optional<size_t>("octaves");
                if (pt_octaves)
                    octaves = *pt_octaves;

                boost::optional<Real> pt_exponent = v.second.get_optional<Real>("exponent");
                if (pt_exponent)
                    exponent = *pt_exponent;

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::FBMOperator<Real, generator_type> fbm_operator{random_number_generator, octaves, exponent, multiplier, scale, offset};
                result = apply_unary_operator(type, v, generator, fbm_operator);
            }
            else if (type == "FillOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::FillOperator<Real> fill_operator{multiplier, scale, offset};
                result = apply_unary_operator(type, v, generator, fill_operator);
            }
            else if (type == "GradientOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::GradientOperator<Real> gradient_operator{multiplier, scale, offset};
                result = apply_unary_operator(type, v, generator, gradient_operator);
            }
            else if (type == "GreaterThanOperator")
            {
                std::vector<Real> level{static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)};

                parse_level(v, level);

                bool clamp{false};
                boost::optional<std::string> pt_mode = v.second.get_optional<std::string>("mode");
                if (pt_mode)
                {
                    if (*pt_mode == "Clamp")
                    {
                        clamp = true;
                    }
                }

                bluedot::GreaterThanOperator<Real> greater_than_operator{level, clamp};
                result = apply_unary_operator(type, v, generator, greater_than_operator);
            }
            else if (type == "LessThanOperator")
            {
                std::vector<Real> level{static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)};

                parse_level(v, level);

                bool clamp{false};
                boost::optional<std::string> pt_mode = v.second.get_optional<std::string>("mode");
                if (pt_mode)
                {
                    if (*pt_mode == "Clamp")
                    {
                        clamp = true;
                    }
                }

                bluedot::LessThanOperator<Real> less_than_operator{level, clamp};
                result = apply_unary_operator(type, v, generator, less_than_operator);
            }
            else if (type == "MultiplyOperator")
            {
                std::vector<Real> multiplier;
                Real scale{static_cast<Real>(1.0)};
                Real offset{static_cast<Real>(0.0)};

                parse_multiplier_scale_and_offset(v, multiplier, scale, offset);

                bluedot::MultiplyOperator<Real> multiply_operator{multiplier, scale, offset};
                result = apply_binary_operator(type, v, generator, multiply_operator);
            }
            else if (type == "SwapOperator")
            {
                bluedot::SwapOperator<Real> swap_operator;
                result = apply_binary_operator(type, v, generator, swap_operator);
            }
            else
            {
                std::cerr << "Unknown operator type: " << type << "\n";
                result = false;
            }
            if (!result)
            {
                std::cerr << "Failed to apply operator of type: " << type << "\n";
            }
        }
    }
    catch (pt::ptree_bad_path& e)
    {
        std::cerr << "Unable to find operators in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (pt::ptree_bad_data& e)
    {
        std::cerr << "Unable to read operators in configuration file.\n";
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "Unhandled exception when reading operators in configuration file.\n";
        return false;
    }
    return true;
}

auto main(int argc, char** argv) -> int
{
    std::cout << "bluedot 1.0" << std::endl;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message")
        ("input,i", po::value<std::string>()->required(), "Input configuration file")
        ("output,o", po::value<std::string>()->required(), "Output file");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (po::required_option& e)
    {
        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }
        else
        {
            std::cerr << "The argument " << e.get_option_name() << " is required to run bluedot.\n";
            return 1;
        }
    }

    const fs::path input_file{vm["input"].as<std::string>()};
    const fs::path output_file{vm["output"].as<std::string>()};

    if (!fs::exists(input_file))
    {
        std::cerr << "Input file " << input_file << " does not exist." << std::endl;
        return 1;
    }

    const std::string input_extension = fs::extension(input_file);

    // Read values from configuration file
    pt::ptree property_tree;

    try
    {
        if (input_extension == ".xml")
        {
            pt::read_xml(input_file.string(), property_tree);
        }
        else if (input_extension == ".json")
        {
            pt::read_json(input_file.string(), property_tree);
        }
        else if (input_extension == ".ini")
        {
            pt::read_ini(input_file.string(), property_tree);
        }
        else if (input_extension == ".info")
        {
            pt::read_info(input_file.string(), property_tree);
        }
        else
        {
            std::cout << "Error: Unknown configuration file extension: " << input_extension << "\n";
            return 1;
        }
    }
    catch (pt::file_parser_error& e)
    {
        std::cerr << "Error: Failed to parse configuration file: " << input_file.string() << "\n";
        std::cerr << e.message() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Error: Failed to parse configuration file: " << input_file.string() << "\n";
        return 1;
    }

    size_t width{1024};
    size_t height{1024};
    size_t seed{4913};

    if (!parse_properties(property_tree, width, height, seed))
    {
        return 1;
    }

    bluedot::Generator<float> generator;

    // Create layers 
    if (!create_layers<float>(property_tree, generator, width, height))
    {
        return 1;
    }

    // Apply operators
    if (!apply_operators(property_tree, generator, seed))
    {
        return 1;
    }

    // Write image
    std::vector<std::array<char, 3>> image{width * height, {0, 0, 0}};
    
    for (size_t y{0}; y < height; ++y)
        for (size_t x{0}; x < width; ++x)
        {
            image[x + y * width][0] = static_cast<char>(std::max(0.0f, std::min(1.0f, generator("base", x, y, 1))) * 255.0f);
            image[x + y * width][1] = static_cast<char>(std::max(0.0f, std::min(1.0f, generator("base", x, y, 2))) * 255.0f);
            image[x + y * width][2] = static_cast<char>(std::max(0.0f, std::min(1.0f, generator("base", x, y, 3))) * 255.0f);
        }

    std::cout << "Writing to " << output_file.string() << std::endl;
    std::ofstream out{output_file.string(), std::ios::out | std::ios::binary};
    out << "P6\n";
    out << "# " << output_file.string() << "\n";
    out << width << " " << height << " 255 ";
    for (size_t y{0}; y < height; ++y)
        for (size_t x{0}; x < width; ++x)
        {
            out << image[x + y * width][0] << image[x + y * width][1] << image[x + y * width][2];
        }

    return 0;
}
