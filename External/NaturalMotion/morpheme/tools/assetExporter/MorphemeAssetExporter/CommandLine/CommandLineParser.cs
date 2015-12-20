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
    /// Utility class to process a command line into a command.
    /// </summary>
    public class CommandLineParser
    {
        public const string SingleLetterPrefix = "-";
        public const string MultiLetterPrefix = "--";
        public const string Quote = "\"";

        /// <summary>
        /// Parses the given parameter according to the following grammar:
        ///     command-line ::= command-name parameter* option*
        ///     option ::= (single-letter-prefix option-name) | (multi-letter-prefix option-name) parameter*
        ///     parameter ::= parameter-name
        ///
        /// That is a command is expected as the first argument followed by any number of parameters followed by any
        /// number of options where each option can have any number of parameters. Another way of looking at this is
        /// that the command is a special option that does not have a prefix and must occur at the start of the command
        /// line. Quoted arguments are merged into a single argument and the quotes are removed.
        /// 
        /// The resulting command can either be used directly to inspect the parameters and options of the command as
        /// represented in the given command-line or validated using a command-line template and invoked.
        /// </summary>
        /// <param name="args">The command line arguments.</param>
        /// <returns>A command object representing the command line.</returns>
        /// <see cref="CommandLineTemplate"/>
        public static Command Parse(string[] args)
        {
            Command command = null;

            // The current context.
            OptionCollection currentOptions = null;
            Utils.NamedParameterList curentParameterList = null;

            List<string> parameterCache = new List<string>(4);
            string quotedParameterCache = null;

            foreach (string arg in args)
            {
                System.Diagnostics.Debug.Assert((arg.Length > 0) && (arg.Trim() == arg));

                // Currently processing the command ...
                if (command == null)
                {
                    ValidateCommand(arg);

                    // Create command.
                    command = new Command(arg);

                    // Set context.
                    currentOptions = command.Options;
                    curentParameterList = command;
                }
                // ... or processing a quoted parameter ...
                else if (quotedParameterCache != null)
                {
                    if (arg.EndsWith(Quote))
                    {
                        quotedParameterCache += " " + arg.Substring(0, (arg.Length - 1));

                        parameterCache.Add(quotedParameterCache);
                        quotedParameterCache = null;
                    }
                    else
                    {
                        quotedParameterCache += " " + arg;
                    }
                }
                // ... or is this the start of a quoted parameter ...
                else if (arg.StartsWith(Quote))
                {
                    quotedParameterCache = arg.Substring(1);
                }
                // ... or an option ...
                else if (arg.StartsWith(SingleLetterPrefix))
                {
                    // Close current option.
                    FlushParameters(curentParameterList, ref parameterCache);

                    // Extract option name.
                    int optionStartIndex = 1;

                    if (arg.StartsWith(MultiLetterPrefix))
                    {
                        ValudateMultiLetterOption(arg);
                        optionStartIndex = 2;
                    }
                    else
                    {
                        ValudateSingleLetterOption(arg);
                    }

                    string optionName = arg.Substring(optionStartIndex);

                    // Add new option (parameter list is cached and set later).
                    Option option = new Option(optionName);
                    currentOptions.Add(option);

                    curentParameterList = option;
                }
                // ... or just a regular parameter.
                else
                {
                    if (curentParameterList == null)
                    {
                        throw new Exception(string.Format("Single- or multi-letter option expected instead of '{0}'!", arg));
                    }

                    parameterCache.Add(arg);
                }
            }

            // Close of previous option.
            FlushParameters(curentParameterList, ref parameterCache);

            // ...
            System.Diagnostics.Debug.Assert(parameterCache.Count == 0);
            System.Diagnostics.Debug.Assert(quotedParameterCache == null);

            return command;
        }

        private static void FlushParameters(Utils.NamedParameterList curentParameterList, ref List<string> parameterCache)
        {
            if (curentParameterList != null)
            {
                System.Diagnostics.Debug.Assert(curentParameterList.Parameters == null);

                curentParameterList.Parameters = parameterCache.ToArray();
                parameterCache.Clear();
            }
        }

        private static void ValidateCommand(string arg)
        {
            if (arg.StartsWith(SingleLetterPrefix) || arg.StartsWith(Quote))
            {
                throw new Exception(string.Format("Valid command expected but got '{0}'!", arg));
            }
        }

        private static void ValudateSingleLetterOption(string arg)
        {
            if (arg.Length != 2)
            {
                throw new Exception(string.Format("Single-letter option expected after '{1}' in '{0}'!", arg, SingleLetterPrefix));
            }
        }

        private static void ValudateMultiLetterOption(string arg)
        {
            if (arg.Length < 4)
            {
                throw new Exception(string.Format("Multi-letter option expected after '{1}' in '{0}'!", arg, MultiLetterPrefix));
            }
        }
    }
}
