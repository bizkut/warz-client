// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

using System;
using System.Collections.Generic;

namespace NaturalMotion.AssetExporter.CommandLine
{
    /// <summary>
    /// Declares a template for a command line including which command and options are valid and which options can be
    /// used in combination with which commands.
    /// </summary>
    /// <example>
    /// CommandLineTemplate commandLine = new CommandLineTemplate("usage: ...")
    ///     .AddOption("v|verbose", "Extra verbose output", (O) => { Verbose = true; })
    ///     .AddOption("s|silent", "No output", (O) => { Verbose = false; Silent = true; })
    ///     .AddCommand("version", "Displays the application version", CommandLineTemplate.IgnoreOptions, OnVersion);
    /// </example>
    public class CommandLineTemplate
    {
        public const string IgnoreOptions = "-";

        public string Description { get; set; }
        public Dictionary<string, OptionTemplate> OptionTemplates { get; private set; }
        public Dictionary<string, CommandTemplate> CommandTemplates { get; private set; }

        public CommandLineTemplate(string description)
        {
            Description = description;
            OptionTemplates = new Dictionary<string, OptionTemplate>();
            CommandTemplates = new Dictionary<string, CommandTemplate>();
        }

        /// <summary>
        /// Adds an option to the template.
        /// </summary>
        /// <param name="nameTemplate">A '|' separated list of valid names for this option.</param>
        /// <param name="description">A description of the option.</param>
        /// <param name="action">The action to invoke for this option (may be null).</param>
        /// <returns>This.</returns>
        public CommandLineTemplate AddOption(string nameTemplate, string description, OptionAction action)
        {
            string[] names = nameTemplate.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);

            OptionTemplate optionTemplate = new OptionTemplate(description, (action != null ? action : (O) => { }));

            foreach (string name in names)
            {
                OptionTemplates.Add(name, optionTemplate);
            }

            return this;
        }

        /// <summary>
        /// Adds a command to the template.
        /// </summary>
        /// <param name="nameTemplate">A '|' separated list of valid names for this option.</param>
        /// <param name="description">A description of the command</param>
        /// <param name="validOptions">A '|' separated list of valid options for this command. Using CommandLineTemplate.IgnoreOptions will accept any option</param>
        /// <param name="action">The action to invoke for this command (may be null).</param>
        /// <returns>This.</returns>
        public CommandLineTemplate AddCommand(string nameTemplate, string description, string validOptions, CommandAction action)
        {
            string[] names = nameTemplate.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);

            CommandTemplate commandTemplate = new CommandTemplate(description, validOptions, (action != null ? action : (O) => { }));

            foreach (string name in names)
            {
                CommandTemplates.Add(name, commandTemplate);
            }

            return this;
        }

        /// <summary>
        /// Validates the given command against this template and fills in the appropriate actions on the command and
        /// options. Invalid commands result in exceptions.
        /// </summary>
        /// <param name="command"></param>
        public void Validate(Command command)
        {
            System.Diagnostics.Debug.Assert(command != null);
            
            // Valid command ?
            if (!CommandTemplates.ContainsKey(command.Name))
            {
                throw new InvalidCommandException(command);
            }

            // Assign command template.
            CommandTemplate commandTemplate = CommandTemplates[command.Name];
            command.Template = commandTemplate;

            // Check options.
            foreach (Option option in command.Options)
            {
                // Valid option ?
                if (!OptionTemplates.ContainsKey(option.Name))
                {
                    throw new InvalidOptionException(option);
                }

                // Assign option template.
                OptionTemplate optionTemplate = OptionTemplates[option.Name];
                option.Template = optionTemplate;
            }

            if (commandTemplate.ValidOptions != IgnoreOptions)
            {
                string[] options = commandTemplate.ValidOptions.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);

                foreach (Option option in command.Options)
                {
                    // Valid options for this command ?
                    if (Array.IndexOf(options, option.Name) < 0)
                    {
                        throw new InvalidOptionException(option);
                    }
                }
            }
        }
    }

    [Serializable]
    public class InvalidCommandException : Exception
    {
        public InvalidCommandException(Command commmand)
            : base(String.Format("Invalid command '{0}'!", commmand.Name))
        {
        }
    }

    [Serializable]
    public class InvalidOptionException : Exception
    {
        public InvalidOptionException(Option option)
            : base(String.Format("Invalid option '{0}'!", option.Name))
        {
        }
    }
}
