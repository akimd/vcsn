.. default-domain:: cpp

Scalar Weights
==============

The following classes implement the :doc:`Weight Set interface <weightsets>`.

.. class:: b

   The classical Boolean semiring :math:`(\mathbb{B},\lor,\land,0,1)`,
   with elements stored as ``bool``.

   The star of any weight is :math:`1`.

   Defined in ``vcsn/weights/b.hh``.

.. class:: z

   The usual integer semiring :math:`(\mathbb{Z},+,\times,0,1)`,
   with elements stored as ``int``.

   The only starable weight is :math:`0` and its star is :math:`1`.

   Defined in ``vcsn/weights/z.hh``.

.. class:: zmin

   .. index:: tropical semiring

   The tropical semiring :math:`(\mathbb{Z}\cup\{\infty\},\min,+,\infty,0)`,
   with elements stored as ``int``.

   All nonnegative weight are starable and their star is :math:`0`.

   Defined in ``vcsn/weights/zmin.hh``.
