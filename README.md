# resistant-server
This is a work in progress for a distributed web server that allows for websites to be displayed on the internet and for files to be stored remotely without worry of being taken down or having interrupted service by having a hosting service that is distributed among the storage space of many hosts similar to p2p. It will also allow for websites to be stored on this main web server therefore creating a resilient space for web hosting.
# Potential Problems
Although very similar to how torrents function this has the necessity of an updated file roster on every host in order to keep track of all files present on the overall web server including added or removed files in accordance with user actions.
# Current Approach
Currently there are two parts in order to have a functional distributed web server:
- first is an attempt to have hosts take a dedicated storage medium or partition and run setup software to prime that storage space for syncronization with other hosts to have a uniform storage space for hosting parts of the same file accross varying storage spaces (think raid 60 configuration)
- second is to have a user system interface which will allow users who wish to add their files or website data onto the distributed system and syncronize these files across all hosts to have the files safely secured in the even should one host be taken down or go offline unexpectedly

# Requirements
Each individual host will have the following:
- A dedicated hard drive or storage space that will contribute to the public web server
- A semi reliable internet connection to connect to other peers for storage syncing
- the proper dependencies from the github repository

Each individual user will have the following
- files or webpages that they wish to store into the distributed web server
- the user interface executable that adds the files into the system across all if not most hosts

# Steps to Complete
1. Host Setup:
    - Develop and implement the host setup software to prime storage space for synchronization.
    - Ensure that hosts can communicate and synchronize storage spaces uniformly.

2. User System Interface:
    - Develop a user-friendly interface for users to add their files or website data onto the distributed system.
    - Implement synchronization functionality for user-added files across all hosts.

3. File Roster Management:
    - Develop a robust system for maintaining an updated file roster on every host.
    - Implement mechanisms to handle added or removed files in accordance with user actions.

4. Host Monitoring and Sync Mechanism:
    - Implement a system to monitor hosts' online/offline status.
    - Develop a sync mechanism to ensure that new files are added when a host comes back online.

5. Testing and Documentation:
    - Test the system thoroughly to identify and fix any bugs or issues.
    - Provide clear documentation for hosts and users on how to set up and use the system.
# Notes
- There must be a check for host that should the go offline and come back online again there is a sync measure to ensure that new files are added
- A setup most like a RAID 60 configuration would be the most likely candidate to ensure that data accross all host storage is well secured while allowing for storage efficiency