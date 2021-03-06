!
! Distributed under the OSI-approved Apache License, Version 2.0.  See
!  accompanying file Copyright.txt for details.
!
!  adios2_file_mod.f90 : ADIOS2 high-level Fortran bindings
!
!   Created on: Feb 28, 2018
!       Author: William F Godoy godoywf@ornl.gov
!
module adios2_file_mod
    use adios2_parameters_mod
    use adios2_fopen_mod
    use adios2_fwrite_mod
    use adios2_fread_mod
    implicit none

contains

    subroutine adios2_fclose(unit, ierr)
        type(adios2_file), intent(inout) :: unit
        integer, intent(out) :: ierr

        call adios2_fclose_f2c(unit%f2c, ierr)
        unit%valid = .false.

    end subroutine

end module
