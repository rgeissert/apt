// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
// $Id: dirstream.cc,v 1.2 2001/02/20 07:03:16 jgg Exp $
/* ######################################################################

   Directory Stream 
   
   This class provides a simple basic extractor that can be used for
   a number of purposes.
   
   ##################################################################### */
									/*}}}*/
// Include Files							/*{{{*/
#ifdef __GNUG__
#pragma implementation "apt-pkg/dirstream.h"
#endif

#include <apt-pkg/dirstream.h>
#include <apt-pkg/error.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <utime.h>
#include <unistd.h>
									/*}}}*/

// DirStream::DoItem - Process an item					/*{{{*/
// ---------------------------------------------------------------------
/* This is a very simple extractor, it does not deal with things like
   overwriting directories with files and so on. */
bool pkgDirStream::DoItem(Item &Itm,int &Fd)
{
   switch (Itm.Type)
   {
      case Item::File:
      {
	 /* Open the output file, NDELAY is used to prevent this from 
	    blowing up on device special files.. */
	 int iFd = open(Itm.Name,O_NDELAY|O_WRONLY|O_CREAT|O_TRUNC|O_APPEND,
		       Itm.Mode);
	 if (iFd < 0)
	    return _error->Errno("open","Failed write file %s",
				 Itm.Name);
	 
	 // fchmod deals with umask and fchown sets the ownership
	 if (fchmod(iFd,Itm.Mode) != 0)
	    return _error->Errno("fchmod","Failed write file %s",
				 Itm.Name);
	 if (fchown(iFd,Itm.UID,Itm.GID) != 0 && errno != EPERM)
	    return _error->Errno("fchown","Failed write file %s",
				 Itm.Name);
	 Fd = iFd;
	 return true;
      }
      
      case Item::HardLink:
      case Item::SymbolicLink:
      case Item::CharDevice:
      case Item::BlockDevice:
      case Item::Directory:
      case Item::FIFO:
      break;
   }
   
   return true;
}
									/*}}}*/
// DirStream::FinishedFile - Finished processing a file			/*{{{*/
// ---------------------------------------------------------------------
/* */
bool pkgDirStream::FinishedFile(Item &Itm,int Fd)
{
   if (Fd < 0)
      return true;
   
   if (close(Fd) != 0)
      return _error->Errno("close","Failed to close file %s",Itm.Name);

   /* Set the modification times. The only way it can fail is if someone
      has futzed with our file, which is intolerable :> */
   struct utimbuf Time;
   Time.actime = Itm.MTime;
   Time.modtime = Itm.MTime;
   if (utime(Itm.Name,&Time) != 0)
      _error->Errno("utime","Failed to close file %s",Itm.Name);
   
   return true;   
}
									/*}}}*/
// DirStream::Fail - Failed processing a file				/*{{{*/
// ---------------------------------------------------------------------
/* */
bool pkgDirStream::Fail(Item &Itm,int Fd)
{
   if (Fd < 0)
      return true;
   
   close(Fd);
   return false;
}
									/*}}}*/