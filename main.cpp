#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

namespace po = boost::program_options;

po::variables_map parse_subcommand(const po::parsed_options& parsed, 
    const po::options_description& description,
    const po::positional_options_description* pos = nullptr)
{
    // Collect all the unrecognized options from the first pass. This will include the
    // (positional) command name, so we need to erase that.
    std::vector<std::string> opts = po::collect_unrecognized(parsed.options, po::include_positional);
    opts.erase(opts.begin());
    po::variables_map vm;
    auto parser = po::command_line_parser(opts)
        .options(description);
    if (pos)
    {
        parser.positional(*pos);
    }
    po::store(parser.run(), vm);
    return vm;
}

void parse_ls(const po::parsed_options& parsed)
{
    string path;
    po::options_description ls_desc("ls options");
    ls_desc.add_options()
        ("path", po::value<std::string>(&path), "Path to list")
        ("extra_option", "Extra option");
    po::variables_map vm = parse_subcommand(parsed, ls_desc);
    if (!vm.count("path"))
    {
        cout << "path not specified" << endl;
    }
    else
    {
        cout << "path(pointer)=" << path << endl;
        cout << "path=" << vm["path"].as<std::string>() << endl;
    }
    auto extra_option = vm.count("extra_option");
    if (extra_option){
        cout << vm["extra_option"].as<string>() << endl;
        cout << "extra option true"<<endl;
    }
}

void parse_subcmd(const po::parsed_options& parsed)
{
    po::options_description desc("subcmd options");
    desc.add_options()
        ("value", po::value<double>(), "value1")
        ("value2", po::value<vector<double>>(), "value2")
        ("value3", po::value<vector<int>>(), "value3");
    po::positional_options_description pos;
    pos.add("value3", -1)
       .add("value2", 2);
    po::variables_map vm = parse_subcommand(parsed, desc, &pos);
    {
        auto s = "value";
        if (vm.count(s))
        {
            cout << s << "=" << vm[s].as<double>() << endl;
        }
    }
    {
        auto s = "value2";
        if (vm.count(s))
        {
            for(auto x : vm[s].as<vector<double>>())
                cout << s << "=" << x << endl;
        }
    }
    {
        auto s = "value3";
        if (vm.count(s))
        {
            for(auto x : vm[s].as<vector<int>>())
                cout << s << "=" << x << endl;
        }
    }
}


void cmd_main(int argc, const char* argv[])
{
    po::variables_map vm;
    // parse main and store it to vm
    po::options_description global("Global options");
    global.add_options()
        ("debug", "Turn on debug output")
        ("command", po::value<std::string>(), "command to execute")
        ("subargs_s", po::value<std::vector<std::string> >(), "Arguments for command");

    po::positional_options_description pos;
    pos.add("command", 1).
        add("subargs_s", -1);


    po::parsed_options parsed = po::command_line_parser(argc, argv).
        options(global).
        positional(pos).
        allow_unregistered().
        run();

    po::store(parsed, vm);

    if (vm.count("debug")){
        cout << "MODE=debug" << endl;
    }

    std::string cmd = vm["command"].as<std::string>();
    if (cmd == "ls")
    {
        parse_ls(parsed);
    }
    else if (cmd == "subcmd")
    {
        parse_subcmd(parsed);
    }
    else
    {
        throw po::invalid_option_value(cmd);
    }
}

int main(int argc, const char* argv[])
{
    cmd_main(argc, argv);
}


