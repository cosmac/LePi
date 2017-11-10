/**
 * Main types and defines for the server application
 */

#pragma once

// Client message
enum STREAMER_MSG { FRAME_REQUEST, // Request a frame [IN/OUT]
                    I2C_CMD,      // Sends an I2C command [IN/OUT]
                    UNKNOWN_MSG    // Response msg for an unknown [OUT]
                  };

// Server response
enum SERVER_RESP { FRAME_READY,   // The response contains a valid frame
                   NO_FRAME,      // No frame ready to be read [We can avoid this by repeating last frame]
                   I2C_SUCCEED,   // I2C command was applied with success
                   I2C_FAILED,    // I2C command failed
                   RESEND         // If the client message was something unknown, ask for a resend
                 };
